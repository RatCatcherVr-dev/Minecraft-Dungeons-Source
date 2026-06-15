#include "Dungeons.h"

#include "LovikaObjectgroupLevelActor.h"
#include "builder/LovikaObjectgroupBuilder.h"
#include "game/GameTypes.h"
#include "client/renderer/texture/TextureAtlas.h"
#include "world/phys/Vec3.h"
#include "world/phys/HitResult.h"
#include "world/level/BlockSource.h"
#include "world/level/material/Material.h"
#include "io/ObjectGroupFile.h"

ALovikaObjectgroupLevelActor::ALovikaObjectgroupLevelActor()
{
	PrimaryActorTick.bCanEverTick = false;

	USphereComponent* SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SphereComponent;
	RootComponent->SetMobility(EComponentMobility::Static);

	static ConstructorHelpers::FClassFinder<AActor> fireBPClassFinder(*(game::PrefabPath("Decor/Prefabs/Blueprints/BP_Fire")));
	if (fireBPClassFinder.Class != nullptr)
	{
		fireBPClass = fireBPClassFinder.Class;
	}

	mBuilder = CreateDefaultSubobject<ULovikaObjectgroupBuilder>(TEXT("ObjectGroupBuilder"));
}

void ALovikaObjectgroupLevelActor::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("Running ALovikaObjectgroupLevelActor::BeginPlay"));

	doRefresh = true;
	
	RebuildLevel(true);
}

void ALovikaObjectgroupLevelActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ALovikaObjectgroupLevelActor::RebuildLevel(bool rebuildMeshes)
{
	if (doRefresh) {
		mBuilder->init({ ResourcePack(resourcePack) });
		DebugAtlasTexture = mBuilder->getMostRecentResourcePackTextureAtlas().atlasTexture;
		DebugAtlasTextureEmissive = mBuilder->getMostRecentResourcePackTextureAtlas().atlasTextureEmissive;	
		mBuilder->build(useAmbientOcclusion, objectGroupName, rebuildMeshes, commaSeparatedTileIdsStringToOptionalArray(tileIds));
		RegisterAllComponents();
		doRefresh = false;
	}
}

void ALovikaObjectgroupLevelActor::OnConstruction(const FTransform& transform)
{
	Super::OnConstruction(transform);

	if (!IsRunningCommandlet())
	{
		UE_LOG(LogTemp, Warning, TEXT("Running ALovikaObjectgroupLevelActor::OnConstruction"));
		RebuildLevel(true);
	}
}

void ALovikaObjectgroupLevelActor::PreInitializeComponents()
{
	UE_LOG(LogTemp, Warning, TEXT("Running ALovikaObjectgroupLevelActor::PreInitializeComponents"));
 	RebuildLevel(false);
}

FLevelHitResult ALovikaObjectgroupLevelActor::levelTrace(FVector from, FVector to, bool liquid, bool solidOnly)
{
	if (mBuilder) {
		BlockSource* region = mBuilder->getBlockSource();
		FVector levelOrigin = GetActorLocation();
		from -= levelOrigin;
		to -= levelOrigin;
		const float scale = 100.0f;
		Vec3 fromInVoxel(from.X / scale, from.Z / scale, from.Y / scale);
		Vec3 toInVoxel(to.X / scale, to.Z / scale, to.Y / scale);
		HitResult hitResult = region->clip(fromInVoxel, toInVoxel, liquid, solidOnly);
		if (hitResult.isTile()) {
			return FLevelHitResult(true, hitResult.isHitLiquid(), static_cast<EMaterialTypeEnum>(region->getMaterial(hitResult.getBlock()).getType()));
		}
	}
	return FLevelHitResult();
}

TOptional<TArray<FString>> commaSeparatedTileIdsStringToOptionalArray(FString tileIds) {
	tileIds = tileIds.Replace(TEXT(" "), TEXT(""));

	if (tileIds.IsEmpty()) {
		return TOptional<TArray<FString>>{};
	}
	TArray<FString> separatedTileIds;
	tileIds.ParseIntoArray(separatedTileIds, TEXT(","), true);
	return separatedTileIds;
}
