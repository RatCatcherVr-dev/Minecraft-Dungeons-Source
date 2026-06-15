#pragma once

#include "GameFramework/Actor.h"
#include "LevelCommon.h"
#include "client/resource/Resource.h"
#include "builder/LovikaObjectgroupBuilder.h"
#include "LovikaObjectgroupLevelActor.generated.h"

namespace builder {
class ULovikaObjectgroupBuilder;
}

UCLASS()
class DUNGEONS_API ALovikaObjectgroupLevelActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ALovikaObjectgroupLevelActor();

	void BeginPlay() override;
	
	void Tick( float DeltaSeconds ) override;

	void OnConstruction(const FTransform& Transform) override;

	void PreInitializeComponents() override;

	
	UFUNCTION(BlueprintCallable, Category = LovikaLevelFeature)
	FLevelHitResult levelTrace(FVector from, FVector to, bool liquid, bool solidOnly);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LovikaLevel)
	TSubclassOf<AActor> fireBPClass;

	/** Dynamically created texture */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = LovikaLevel)
	UTexture2D* DebugAtlasTexture;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = LovikaLevel)
	UTexture2D* DebugAtlasTextureEmissive;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LovikaLevel)
	bool useAmbientOcclusion = true;

	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, Category = LovikaLevel)
	bool doRefresh = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LovikaLevel)
	FString objectGroupName = "ny_hel_bana2.json";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LovikaLevel)
	EResourcePack resourcePack = EResourcePack::Vanilla;

	/** Comma separated list of tile IDs to load. Leave empty to load all tiles. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LovikaLevel)
	FString tileIds = "";
private:	
	void RebuildLevel(bool rebuildMeshes);

	ULovikaObjectgroupBuilder* mBuilder = nullptr;
};

TOptional<TArray<FString>> commaSeparatedTileIdsStringToOptionalArray(FString);
