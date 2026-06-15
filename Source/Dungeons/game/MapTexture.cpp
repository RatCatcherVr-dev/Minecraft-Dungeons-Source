#include "Dungeons.h"
#include "MapTexture.h"
#include "lovika/world/level/terrain/Terrain.h"
#include "GameBP.h"
#include "Conversion.h"
#include "actor/character/loot/LootActor.h"
#include "game/util/PlayerQuery.h"
#include <Materials/MaterialInstance.h>
#include <Materials/MaterialLayersFunctions.h>
#include <Private/Materials/MaterialInstanceSupport.h>

#include "Engine/Engine.h"
#include <cstring>

TAutoConsoleVariable<int32> CVarRevealAll(
	TEXT("Dungeons.Level.RevealAll"),
	0, 
	TEXT("Reveal the entire map.\n")
	TEXT("<= 0: off.\n")
	TEXT(">= 1: on.\n"),
	ECVF_Cheat
);

void UMapTexture2D::initTexture(int width, int height, EMapTextureSampler samplerMode)
{
	mWidth = width;
	mHeight = height;
	mSamplerMode = samplerMode;
	mSamplerSize = [&]() {
		switch (samplerMode) {
		case EMapTextureSampler::Size2x2:
			return 2;			
		case EMapTextureSampler::Size1x1:
		default:
			return 1;
		}
	}();

	mChannels = 2;

	mTexture = UTexture2D::CreateTransient(width, height, EPixelFormat::PF_R8G8);
	mTexture->SRGB = false;
	mTexture->Filter = TextureFilter::TF_Nearest;
	mTexture->CompressionSettings = TextureCompressionSettings::TC_EditorIcon;
	mTexture->LODGroup = TEXTUREGROUP_UI;
	mTexture->AddressX = TA_Clamp;
	mTexture->AddressY = TA_Clamp;
	
	mTexture->UpdateResource();

	// D11.DH
	// changed to manual allocation instead of a vector as the vector was going out of scope during the update texture regions async task
	// used QuantizeSize to avoid fragmenting the ue4 memory pool
	mSizeOffBuffer = GMalloc->QuantizeSize(mWidth*mHeight*mChannels, std::alignment_of<uint8>());
	mData = (uint8*)GMalloc->Malloc(mSizeOffBuffer);
	// D11.DH 
	// 0 out the map data to avoid fragmented data being displayed
	std::memset(mData, 0, mSizeOffBuffer);
	clearTexture(mData);
}

FVector2D UMapTexture2D::worldToMap(const FVector& worldLocation, const FVector& center) const
{
	const auto scale = FVector2D{ static_cast<float>(mWidth), static_cast<float>(mHeight) } *Math::PE_TO_UE_UNITS * mSamplerSize;
	return (FVector2D{ worldLocation } -FVector2D{ center }) / scale + .5f;
}


FVector2D UMapTexture2D::getLocationUV(const FVector& worldLocation) const {
	const auto pixelSize = Math::PE_TO_UE_UNITS * mSamplerSize;
	return FVector2D{ worldLocation.X / pixelSize / static_cast<float>(getWidth()), worldLocation.Y / pixelSize / static_cast<float>(getHeight()) };
}

TArray<UMapPinComponent*> UMapTexture2D::getAllMapPinActors(const FVector& center) const {
	return getActorClassPins(center);
}



TArray<UMapPinComponent*> UMapTexture2D::updateAndGetAllMapPinActors(const FVector& center, EMapType MapType)
{
	TArray<UMapPinComponent*> updatablePins = getActorClassPins(center);
	APlayerCharacter* primaryPlayer = nullptr;
	if (UWorld* world = GetWorld())
	{
		primaryPlayer = playerquery::getFirstLocalPlayerCharacter(world);
	}

	for (auto* comp : updatablePins)
	{
		if (comp)
		{
			comp->Update(MapType, primaryPlayer);
		}
	}

	return updatablePins;
}

int UMapTexture2D::getWidth() const
{
	return mWidth;
}

int UMapTexture2D::getHeight() const
{
	return mHeight;
}

UTexture2D* UMapTexture2D::getTexture() const
{
	return mTexture;
}



void UMapTexture2D::SetDisplayedMaterialInstance(UMaterialInstanceDynamic* materialInstance) {
	mDisplayedMaterialInstance = materialInstance;
}

UMapTexture2D::~UMapTexture2D()
{
	GMalloc->Free(mData);
}

const float UMapTexture2D::UPDATING_TEXTURE_MAP_MAX_WAIT_SECONDS = 4.0f;

namespace maptextureinternal {

inline uint8_t encodeTerrain(TerrainCell cell, EMapHeightThreshold heightOffset) {
	return cell.isReachable() ? static_cast<uint8>(heightOffset) : 0;
}

inline uint8_t encodeReveal(RevealCell cell) {
	return cell == RevealCell::Fog ? 0
	     : cell == RevealCell::PartialReveal ? 1
	     : 99;
};

BlockCuboid toBlockCuboidXz(IntRect rect) {
	return BlockCuboid::fromPositionAndSize({ rect.x, 0, rect.y }, { rect.w, 1, rect.h });
}

IntRect toIntRectXz(BlockCuboid volume) {
	const auto size = volume.size();
	return IntRect{ volume.minInclusive.x, volume.minInclusive.z, size.x, size.z };
}

}

void UMapTexture2D::UpdateTerrain(AGameBP* gameBp, const FVector& center) {

	if (mCurrentlyUpdatingMapTexture && (GetWorld()->GetTimeSeconds() < (mUpdatingMapTextureStampSeconds + UPDATING_TEXTURE_MAP_MAX_WAIT_SECONDS)))
	{
		//don't bother if we are in the process of updating - but wait a maximum of UPDATING_TEXTURE_MAP_WAIT_SECONDS.... just in case.
		return;
	}

	QUICK_SCOPE_CYCLE_COUNTER(STAT_UMapTexture2D_UpdateTerrain_)
	TerrainPos centerTerrainPos = conversion::ueToTerrain(center);

	if (
		!gameBp->GetTerrain()->wasDirty() &&
		(centerTerrainPos.x & 0xFFFFFFFE) == (mLastTerrainPos.x & 0xFFFFFFFE) &&
		(centerTerrainPos.y & 0xFFFFFFFE) == (mLastTerrainPos.y & 0xFFFFFFFE)
	) {
		return;
	}

	mCurrentlyUpdatingMapTexture = true;
	mUpdatingMapTextureStampSeconds = GetWorld()->GetTimeSeconds(); //Fallback timer.
	TWeakObjectPtr<UMapTexture2D> WeakThisObject = this;

	auto* pTask = new FAutoDeleteAsyncTask<FCallbackAsyncTask>([WeakThisObject, center, gameBp]() {
		if (!WeakThisObject.IsValid()) {
			return;
		}
		const auto bounds = [&]() -> TOptional<IntRect> {
			if (auto* game = gameBp->GetGame()) {
				if (auto* tile = game->tiles().getClosestTile(conversion::ueToBlock(center))) {
					return maptextureinternal::toIntRectXz(game->tiles().getSubDungeonInfo(tile->dungeon()).bounds);
				}
			}
			return {};
		}();
		WeakThisObject->UpdateTerrainInternal(center, gameBp->GetTerrain(), bounds);
	});
	pTask->StartBackgroundTask();
}

void UMapTexture2D::UpdateTerrainInternal(const FVector& center, Terrain* terrain, TOptional<IntRect> worldBounds)
{
	// not sure how involved GetValueOnGameThread is;
	// it's probably worth caching its result here and not calling it for every cell of the map
	const bool revealAll = CVarRevealAll.GetValueOnAnyThread() != 0;
	const auto revealOverride = revealAll ? maptextureinternal::encodeReveal(RevealCell::FullReveal) : 0;

	const BlockCuboid mapBounds = [&] {
		const BlockPos centerBlockPos = mSamplerMode == EMapTextureSampler::Size1x1 ? conversion::ueToBlock(center) : conversion::ueToBlock(center) / 2;
		const BlockPos topLeftBlockPos = centerBlockPos - BlockPos{ mWidth / 2 , 0, mHeight / 2 };
		return BlockCuboid::fromPositionAndSize({ topLeftBlockPos.x, 0, topLeftBlockPos.z }, { mWidth, 1, mHeight });
	}();

	const auto scaleWorldBounds = [mode = mSamplerMode](BlockCuboid v) { return mode == EMapTextureSampler::Size1x1 ? v : 
		BlockCuboid::fromInclusiveCorners(v.minInclusive / 2, v.maxInclusive() / 2); };
	const BlockCuboid bounds = worldBounds ? scaleWorldBounds(maptextureinternal::toBlockCuboidXz(worldBounds.GetValue())).intersection(mapBounds) : mapBounds;
	

	if (bounds != mapBounds) {
		check(mSizeOffBuffer > 0);
		std::memset(mData, 0, mSizeOffBuffer);
	}

	int p{ 0 };

	auto HeightOffset = [&](const TerrainPos& terrainPos) -> EMapHeightThreshold
	{
		const uint16_t terrainHeight = mSamplerMode == EMapTextureSampler::Size1x1 ? terrain->getHeight(terrainPos) : terrain->getHeightMag(terrainPos);
		const BlockPos centerBlockPos = conversion::ueToBlock(center);
		const uint16_t clampedCenterBlockHeight = centerBlockPos.y - (centerBlockPos.y % HeightThreshold);
		const uint16_t delta = FMath::Abs(clampedCenterBlockHeight - terrainHeight);

		if (terrainHeight > clampedCenterBlockHeight) {
			return delta > HeightThreshold ? EMapHeightThreshold::AboveThreshold : EMapHeightThreshold::WithinThreshold;
		}
		else {
			return delta > HeightThreshold ? EMapHeightThreshold::BelowThreshold : EMapHeightThreshold::WithinThreshold;
		}
	};

	//D11.RS
	//Due to PS4 not sometimes displaying the very edge of the map texture 
	//this is a fix to test whether or not having an additional + 1 to the index is not going to exceed the buffer size
	int offsetZ = 0;
	int offsetX = 0;
	int maxBoundaryZ = mChannels * ((bounds.maxExclusive.z - mapBounds.minInclusive.z) * (mWidth) + (bounds.minInclusive.x - mapBounds.minInclusive.x));
	if (maxBoundaryZ < mSizeOffBuffer)
	{
		offsetZ = 1;
		int maxBoundaryX = ((bounds.maxExclusive.x - bounds.minInclusive.x) * mChannels) + maxBoundaryZ;
		if (maxBoundaryX < mSizeOffBuffer)
		{
			offsetX = 1;
		}
	}

	switch (mSamplerMode) {
	case EMapTextureSampler::Size1x1:
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_UMapTexture2D_UpdateTerrain_1x1)

		for (int z = bounds.minInclusive.z; z < bounds.maxExclusive.z + offsetZ; ++z) {
			p = mChannels * ((z - mapBounds.minInclusive.z) * mWidth + (bounds.minInclusive.x - mapBounds.minInclusive.x));
			for (int x = bounds.minInclusive.x; x < bounds.maxExclusive.x + offsetX; ++x, p += mChannels) {
				const TerrainPos terrainPos{ x, z };

				setBuffer(p + 0, maptextureinternal::encodeTerrain(terrain->getType(terrainPos), HeightOffset(terrainPos)));
				setBuffer(p + 1, revealOverride ? revealOverride : maptextureinternal::encodeReveal(terrain->getReveal(terrainPos)));
			}
		}
		break;
	}
	case EMapTextureSampler::Size2x2:
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_UMapTexture2D_UpdateTerrain_2x2)

		for (int z = bounds.minInclusive.z; z < bounds.maxExclusive.z + offsetZ; ++z) {
			p = mChannels * ((z - mapBounds.minInclusive.z) * (mWidth) + (bounds.minInclusive.x - mapBounds.minInclusive.x));
			for (int x = bounds.minInclusive.x; x < bounds.maxExclusive.x + offsetX; ++x, p += mChannels) {
				const TerrainPos terrainPos{ x, z };

				setBuffer(p + 0, maptextureinternal::encodeTerrain(terrain->getTypeMag(terrainPos), HeightOffset(terrainPos)));
				setBuffer(p + 1, revealOverride ? revealOverride : maptextureinternal::encodeReveal(terrain->getRevealMag(terrainPos)));
			}
		}
		break;
	}
	}

	mLastTerrainPos = conversion::ueToTerrain(center);

	TWeakObjectPtr<UMapTexture2D> WeakThisObject = this;	
	FFunctionGraphTask::CreateAndDispatchWhenReady([WeakThisObject, center]()
	{
		if (WeakThisObject.IsValid())
		{
			WeakThisObject->updateTexture(WeakThisObject->mData, center);
		}

	}, TStatId(), nullptr, ENamedThreads::GameThread);

	
}

void UMapTexture2D::updateTexture(const uint8* newData, const FVector& center) {
	if (!mDisplayedMaterialInstance) {
		mCurrentlyUpdatingMapTexture = false;
		return;
	}
	// D11.DH
	// now that the lifetime of the buffer is correctly handled UpdateTextureRegions now works on its own and we can leave the texture tiled
	// this should improve performance on consoles
	size_t sizeToAvoidFragmentation = GMalloc->QuantizeSize(sizeof(FUpdateTextureRegion2D), std::alignment_of<FUpdateTextureRegion2D>());
	FUpdateTextureRegion2D *mapRegion = (FUpdateTextureRegion2D*)GMalloc->Malloc(sizeToAvoidFragmentation, std::alignment_of<FUpdateTextureRegion2D>()); 
	mapRegion->SrcX = 0;
	mapRegion->SrcY = 0;

	mapRegion->DestX = 0;
	mapRegion->DestY = 0;
	
	mapRegion->Width = mTexture->GetSizeX();
	mapRegion->Height = mTexture->GetSizeY();
	
	TWeakObjectPtr<UMapTexture2D> WeakThisObject = this;
	const auto pixelSize = Math::PE_TO_UE_UNITS * mSamplerSize;
	TFunction<void(uint8* SrcData, const FUpdateTextureRegion2D* Regions)> MapCleanUpFunc =
		[
			WeakThisObject,
			materialInstanceResource = mDisplayedMaterialInstance->Resource,
			uOffsetParameter = FMaterialParameterInfo("uOffset"),
			vOffsetParameter = FMaterialParameterInfo("vOffset"),
			nextTextureCenterUVWorldLocation = FVector2D{ FMath::FloorToFloat(center.X / pixelSize) / static_cast<float>(getWidth()) , FMath::FloorToFloat(center.Y / pixelSize) / static_cast<float>(getHeight()) }
		](uint8* SrcData, const FUpdateTextureRegion2D* Regions)
	{
		materialInstanceResource->RenderThread_UpdateParameter(uOffsetParameter, nextTextureCenterUVWorldLocation.X);
		materialInstanceResource->RenderThread_UpdateParameter(vOffsetParameter, nextTextureCenterUVWorldLocation.Y);
		GMalloc->Free((void*)Regions);
		if (WeakThisObject.IsValid())
		{
			WeakThisObject->mCurrentlyUpdatingMapTexture = false;
		}
	};
		
	mTexture->UpdateTextureRegions(0, 1, mapRegion, mWidth * mChannels, mChannels, (uint8*)newData, MapCleanUpFunc);
}

void UMapTexture2D::clearTexture(const uint8* newData)
{
	// D11.DH
	// now that the lifetime of the buffer is correctly handled UpdateTextureRegions now works on its own and we can leave the texture tiled
	// this should improve performance on consoles
	size_t sizeToAvoidFragmentation = GMalloc->QuantizeSize(sizeof(FUpdateTextureRegion2D), std::alignment_of<FUpdateTextureRegion2D>());
	FUpdateTextureRegion2D *mapRegion = (FUpdateTextureRegion2D*)GMalloc->Malloc(sizeToAvoidFragmentation, std::alignment_of<FUpdateTextureRegion2D>());
	mapRegion->SrcX = 0;
	mapRegion->SrcY = 0;

	mapRegion->DestX = 0;
	mapRegion->DestY = 0;

	mapRegion->Width = mTexture->GetSizeX();
	mapRegion->Height = mTexture->GetSizeY();

	TWeakObjectPtr<UMapTexture2D> WeakThisObject = this;
	const auto pixelSize = Math::PE_TO_UE_UNITS * mSamplerSize;
	TFunction<void(uint8* SrcData, const FUpdateTextureRegion2D* Regions)> MapCleanUpFunc =
		[](uint8* SrcData, const FUpdateTextureRegion2D* Regions) {
		GMalloc->Free((void*)Regions);
		
	};

	mTexture->UpdateTextureRegions(0, 1, mapRegion, mWidth * mChannels, mChannels, (uint8*)newData, MapCleanUpFunc);
}

void UMapTexture2D::setBuffer(int offset, uint8_t data)
{
	check(mSizeOffBuffer > 0 && offset >= 0 && offset < mSizeOffBuffer);
	mData[offset] = data;
}