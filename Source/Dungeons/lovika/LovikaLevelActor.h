#pragma once

#include "GameFramework/Actor.h"
#include "CommonTypes.h"
#include "LevelCommon.h"
#include "game/Game.h"
#include "game/levels.h"
#include "editor/debug/RegionRenderer.h"
#include "game/LevelSettings.h"
#include "builder/LovikaGeneratorBuilder.h"
#include "LovikaLevelActor.generated.h"

class ChunkBlockSource;
class ResourcePack;

UCLASS()
class DUNGEONS_API ALovikaLevelActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALovikaLevelActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void PreInitializeComponents() override;

	virtual void Tick(float DeltaSeconds) override;

	void setupGameAndBuildMesh(game::Game&, ChunkBlockSource&, const std::vector<ResourcePack>&);

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
	ELevelNames developmentLevelId = ELevelNames::squidcoast;

	///Leave empty to automatically convert LevelId.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LovikaLevel)
	FString developmentLevelFilename = FString();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LovikaLevel)
	EGameDifficulty developmentDifficulty = EGameDifficulty::Difficulty_1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LovikaLevel)
	EThreatLevel developmentThreatLevel = EThreatLevel::Threat_2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LovikaLevel)
	FEndlessStruggle developmentEndlessStruggle;

	///Use true to play with emergent difficulty
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LovikaLevel)
	FEmergentDifficulty developmentEmergentDifficulty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (InlineEditConditionToggle), Category = LovikaLevel)
	bool developmentUseFixedSeed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition="developmentUseFixedSeed"), Category = LovikaLevel)
	int32 developmentFixedLevelSeed = 0;

	const TOptional<FLevelSettings>& getGeneratedLevelSettings() const;

	UFUNCTION(BlueprintCallable, Category = Dungeons)
	FLevelSettings getGeneratedLevelSettingsOrEmpty() const;

	void setGeneratedLevelSettings(const FLevelSettings& levelSettings);

	bool IsShowingRegions() const;
	void SetShowingRegions(bool show);

	/** Searches components array and returns first encountered component of the specified class. */
	virtual UActorComponent* FindComponentByClass(const TSubclassOf<UActorComponent> ComponentClass) const;
	bool IsReady() const;
	void AbortMeshGeneration();

private:
	bool AreAllLocalPlayersLoaded() const;
	void CharactersReadyToPlay();
	bool ShouldPlayIntro() const;
	void rebuildTextures();
	void buildInstancedLeaves(const game::Tiles&);

	bool isShowingRegions = false;
	Unique<RegionRenderer> mRegionRenderer;

	TOptional<FLevelSettings> generatedLevelSettings;

	ULovikaGeneratorBuilder* mBuilder = nullptr;
	game::Game* mGame = nullptr;
	ChunkBlockSource* mRegion = nullptr;
	std::vector<ResourcePack> mResourcePacks;

	int mFramesWithoutPendingMobs = 0;
};
