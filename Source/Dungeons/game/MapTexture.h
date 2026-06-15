#pragma once

#include "Dungeons.h"
#include "lovika/Grid2.h"
#include "lovika/world/level/terrain/TerrainPos.h"
#include "lovika/world/level/terrain/TerrainCell.h"
#include "lovika/world/level/terrain/RevealCell.h"
#include "game/level/chests/ChestType.h"
#include "game/item/ItemRarity.h"
#include "component/MapPinComponent.h"
#include <GameFramework/Actor.h>
#include "MapTexture.generated.h"

class AGameBP;
class Terrain;


UENUM(BlueprintType)
enum class EMapTextureSampler : uint8 {
	Size1x1,
	Size2x2,
};

UENUM()
enum class EMapHeightThreshold : uint8 {
	BelowThreshold = 1,
	WithinThreshold = 2,
	AboveThreshold = 3
};

UCLASS(BlueprintType)
class DUNGEONS_API UMapTexture2D : public UObject
{
	GENERATED_BODY()

public:
	
	~UMapTexture2D();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void UpdateTerrain(AGameBP* gameBp, const FVector& center);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FVector2D worldToMap(const FVector& worldLocation, const FVector& center) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FVector2D getLocationUV(const FVector& worldLocation) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	UTexture2D* getTexture() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void SetDisplayedMaterialInstance(UMaterialInstanceDynamic* materialInstance);

	TArray<UMapPinComponent*> getActorClassPins(const FVector& center) const
	{
		TArray<TWeakObjectPtr<UMapPinComponent>> weakPins;
		TArray<UMapPinComponent*> ptrPins;
		weakPins.Append(InstanceTracker<UMapPinComponent>::GetList(GetWorld()));
		ptrPins.Reserve(weakPins.Num());
		for (TWeakObjectPtr<UMapPinComponent> pin : weakPins)
		{
			if (pin.IsValid() && !pin.IsStale() && pin.Get() && !pin.Get()->IsPendingKillOrUnreachable() && pin.Get()->GetOwner() && !pin.Get()->GetOwner()->IsPendingKillOrUnreachable())
			{
				pin.Get()->setCenterPos(center);
				ptrPins.Emplace(pin.Get());
			}
		}
		return ptrPins;
	}

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	TArray<UMapPinComponent*> getAllMapPinActors(const FVector& center) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	TArray<UMapPinComponent*> updateAndGetAllMapPinActors(const FVector& center, EMapType MapType);

	int getWidth() const;

	int getHeight() const;

	void initTexture(int width, int height, EMapTextureSampler samplerMode);

private:
	void UpdateTerrainInternal(const FVector& center, Terrain*, TOptional<IntRect> inclusionBounds);

	void updateTexture(const uint8* newData, const FVector& center);

	void clearTexture(const uint8* newData);

	UPROPERTY()
	UTexture2D* mTexture;

	UPROPERTY()
	UMaterialInstanceDynamic* mDisplayedMaterialInstance;


	int mWidth;

	int mHeight;

	int mChannels;

	EMapTextureSampler mSamplerMode;

	int mSamplerSize;

	TerrainPos mLastTerrainPos;

	uint8* mData = nullptr;
	size_t mSizeOffBuffer = 0;

	FThreadSafeBool mCurrentlyUpdatingMapTexture = false;
	float mUpdatingMapTextureStampSeconds = 0.f;

	static const float UPDATING_TEXTURE_MAP_MAX_WAIT_SECONDS;

	uint8 HeightThreshold = 6;

	void setBuffer(int offset, uint8_t data);
};

