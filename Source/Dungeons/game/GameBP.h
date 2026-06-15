#pragma once

#include "actor/character/player/PlayerCharacter.h"
#include "game/Game.h"
#include "level/GameLevelDef.h"
#include "lovika/world/level/ChunkBlockSource.h"
#include "lovika/world/level/levelgen/TileSet.h"
#include "lovika/world/level/terrain/Terrain.h"
#include "lovika/BlockCuboid.h"
#include <Platform.h>
#include "LevelSettings.h"
#include "game/affector/AffectorTypes.h"
#include "game/mission/trial/TrialTypes.h"
#include "game/objective/ObjectiveLocations.h"
#include "actor/ObjectiveRing.h"
#include "DungeonsGameInstance.h"
#include <GameFramework/Info.h>

#include "client/resource/Resource.h"
#include "game/mobspawn/MobSpawner.h"
#include "difficulty/Difficulty.h"
#include "game/component/MobLoaderComponent.h"
#include "GameBP.generated.h"


namespace postprocess { struct Config; }

UENUM(BlueprintType)
enum class NavAreaType : uint8 {
	Ground,
	Lava
};

USTRUCT(BlueprintType)
struct FNavigationArea
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Navigation")
	NavAreaType AreaType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Navigation")
	TSubclassOf<class AStaticMeshActor> NavigationModifierActor;

	FNavigationArea()
		: AreaType(NavAreaType::Ground)
	{}
};

struct LevelDataStruct
{
	LevelDataStruct() {}

	Unique<game::LevelDef> mLevelDef;
	Unique<ChunkBlockSource> mRegion;
	std::vector<BlockCuboid> mFilledDoors;
	TileSet mTileSet;

};

struct TerrainRevealUpdateData
{
	TerrainRevealUpdateData();
	TerrainRevealUpdateData(TWeakObjectPtr<APlayerCharacter> playerChar, const TerrainPos& tPos, float radiusScale, const TerrainPos& delta);

	TerrainPos mTPos;
	TerrainPos mDelta;
	float mRadiusScale;
	TWeakObjectPtr<APlayerCharacter> mPlayerChar;
};


struct MobEndSpawnData
{
	MobEndSpawnData() {};
	MobEndSpawnData(const FTransform& inWorldPos, const TArray<FEnchantmentData>& enchantmentData, const game::mobspawn::Config& inMobSpawnConfig, std::function< void(AMobCharacter*) > inPostSpawnFunc) : SpawnedMobCharacter(nullptr), WorldTransform(inWorldPos), EnchantmentData(enchantmentData), MobSpawnConfig(inMobSpawnConfig), PostSpawnFunc(inPostSpawnFunc) {}

	AMobCharacter*							SpawnedMobCharacter;
	FTransform								WorldTransform;
	TArray<FEnchantmentData>				EnchantmentData;
	game::mobspawn::Config					MobSpawnConfig;
	std::function< void(AMobCharacter*) >	PostSpawnFunc;
};


struct MobSpawnData
{
	MobSpawnData() {};
	MobSpawnData(EntityType inMobType, UClass* cls, const FTransform& inWorldPos, const TArray<FEnchantmentData>& enchantmentData, const game::mobspawn::Config& inMobSpawnConfig, std::function< void(AMobCharacter*) > inPostSpawnFunc);

	EntityType								MobType;
	UClass*									MobClass;
	TSharedPtr<FStreamableHandle>			LoadRequest;
	MobEndSpawnData							MobEndData;

};

struct MobGroupSpawnData
{
	using MobSpawnCallback = std::function< void(AMobCharacter*) >;
	using GroupSpawnCallback = std::function< void(TArray<AMobCharacter*>&) >;

	MobGroupSpawnData() {};
	MobGroupSpawnData(EntityType, TArray<FTransform>&&, const game::mobspawn::Config&, MobSpawnCallback, GroupSpawnCallback, const game::mobspawn::SpawnGroup&);

	EntityType										MobType;
	TArray< FTransform >							MobTransforms;
	game::mobspawn::Config							MobSpawnConfig;
	MobSpawnCallback								PostIndividualSpawnFunc;
	GroupSpawnCallback								PostGroupSpawnFunc;
	TArray<FEnchantmentData>						EnchantmentData;
	float											GroupLootDropChance;
	int												MobSpawnCount;

	TArray<TWeakObjectPtr<AMobCharacter>>			MobList;
	TSharedPtr<FStreamableHandle>					LoadRequest;
};


struct MobSpawnQueue
{
	MobSpawnQueue() {};

	TQueue< TSharedPtr<MobSpawnData> > mMobSpawnQueue;
	TQueue< MobEndSpawnData > mMobEndSpawnQueue;

	bool ProcessMobSpawnQueue(UWorld* pWorld, bool bLoadingScreenActive);
	bool AnyPendingSpawns()const { return (!mMobSpawnQueue.IsEmpty() || !mMobEndSpawnQueue.IsEmpty()); };
};



class FTerrainRevealUpdateAsyncTask : public FNonAbandonableTask
{
public:
	friend class FAsyncTask<FTerrainRevealUpdateAsyncTask>;

	Terrain* mTargetTerrain;
	TQueue< TerrainRevealUpdateData >& mUpdateDataList;


	FTerrainRevealUpdateAsyncTask(Terrain* pTargetTerrain, TQueue< TerrainRevealUpdateData >& UpdateList)
		: mTargetTerrain(pTargetTerrain),
		mUpdateDataList(UpdateList)
	{
	}

	void DoWork()
	{
		TerrainRevealUpdateData DequeueDataInst;

		//Update terrain data
		while (mUpdateDataList.Dequeue(DequeueDataInst))
		{
			int iBlocks = mTargetTerrain->revealAround(DequeueDataInst.mTPos, DequeueDataInst.mRadiusScale, DequeueDataInst.mDelta);
			if (iBlocks && DequeueDataInst.mPlayerChar.IsValid())
			{
				DequeueDataInst.mPlayerChar->NotifyBlocksRevealed(iBlocks);
			}
		}
	}

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FTerrainRevealUpdateAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
	}
};


class FLevelGenerationAsyncTask : public FNonAbandonableTask
{
public:
	friend class FAsyncTask<FLevelGenerationAsyncTask>;

	LevelDataStruct& mLevelData;
	FLevelSettings mInitialisingLevelSettings;

	FLevelGenerationAsyncTask(LevelDataStruct& LevelData, FLevelSettings& InitialisingLevelSettings)
		: mLevelData(LevelData),
		mInitialisingLevelSettings(InitialisingLevelSettings)
	{
	}

	void DoWork();

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FLevelGenerationAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
	}
};



UCLASS(Blueprintable, BlueprintType)
class DUNGEONS_API UMeshFadeBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMeshFadeBaseComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void SetMeshHidden(bool Hidden);
};

struct FadeActorCheckPlayerData
{
	FadeActorCheckPlayerData() : mCapsuleRadius(0.0f), mCapsuleHalfHeight(0.0f){};
	TWeakObjectPtr<APlayerCharacter> mPlayerChar;
	FVector mPlayerLocation;
	float mCapsuleRadius;
	float mCapsuleHalfHeight;
};


class FFadeActorCheckAsyncTask : public FNonAbandonableTask
{
public:
	friend class FAsyncTask<FFadeActorCheckAsyncTask>;

	FVector mCurrentCameraPosition;

	TArray < FadeActorCheckPlayerData > mCurrentPlayerActors;

	TArray< AActor* > mCurrentBlockingActors;
	TArray< AActor* > mCurrentHiddenActors;

	TWeakObjectPtr<UWorld> mWorld;

	TWeakObjectPtr<AActor> mLevelActor;

	FFadeActorCheckAsyncTask()
	{
	}

	void DoWork();

	void GameThreadProcessActors();


	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FFadeActorCheckAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
	}
};









UENUM()
enum class AGameBPInternal_EServerLoadingState {
	Loading,
	ReadyForPlayers,
	Started
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameSettingsInitiallized);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChallengeRewardLocationsChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnObjectiveLocationsChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActiveAffectorInfoChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLocalGameplayStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAffectorsChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStrongholdStarted, int, totalEyesOfEnder);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDifficultyIncreased, int, difficultyLevelIncreased);

DECLARE_MULTICAST_DELEGATE(FOnTrialMissionsUpdated);

class UMissionPresentationHandler;

UCLASS(Blueprintable)
class AGameBP : public AInfo {
	GENERATED_BODY()

public:
	AGameBP();
	~AGameBP();

	void Tick(float deltaSeconds) override;

	void UpdateTerrainReveals();
	void UpdateMobSpawn();
	void UpdateFadeActors(const float deltaSeconds);

	void UpdateCharacterDestroy();

	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	void PostRegisterAllComponents() override;
	void PostUnregisterAllComponents() override;

	void InitTrials();

	void SetupPlaySession();
	void SetupAffectors();

	game::Game* GetGame() const;

	game::LevelDef* GetLevelDef() const;

	ChunkBlockSource* BlockSourceWithLoadedMaterials() const;
	ChunkBlockSource* BlockSource() const;

	UFUNCTION(BlueprintCallable)
	bool IsPlayerInRangeOfObjective(APlayerCharacter* player) const;

	UFUNCTION(BlueprintPure, Category = "Default")
	bool GetPreviousDoor(const APlayerCharacter* player, FVector& outPosition) const;

	UFUNCTION(BlueprintPure, Category = "Default")
	bool GetNextDoor(const APlayerCharacter* player, FVector& outPosition) const;

	///Find the entrance on the NEXT tile to the current tile.
	UFUNCTION(BlueprintPure, Category = "Default")
	bool GetNextTileDoor(const APlayerCharacter* player, FVector& outPosition) const;

	///Find the entrance on the PREVIOUS tile to the current tile.
	UFUNCTION(BlueprintPure, Category = "Default")
	bool GetPreviousTileDoor(const APlayerCharacter* player, FVector& outPosition) const;

	bool GetNextDoorIndicatorDoor(const APlayerCharacter*, FVector& out) const;

	UFUNCTION(BlueprintPure, Category = "Dungeons")
	bool GetTriggerRegionCenter(FString regionName, FVector& outPosition) const;

	std::string GetCurrentLevelId() const;

	UFUNCTION(BlueprintCallable, Category="Dungeons")
	UMissionPresentationHandler* GetPresentationHandler() { return mPresentationHandler; }

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Navigation")
	TArray<FNavigationArea>	NavigationAreaTypes;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual const FObjectiveLocations& GetObjectiveLocations();

	io::ObjectiveConsiderTilesType GetObjectiveConsiderTilesType() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const TArray<FVector>& GetChallengeRewardLocations() const;

	FOnGameSettingsInitiallized OnGameSettingsInitiallized;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnChallengeRewardLocationsChanged OnChallengeRewardLocationsChanged;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnObjectiveLocationsChanged OnObjectiveLocationsChanged;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnActiveAffectorInfoChanged OnActiveAffectorInfoChanged;

	FOnTrialMissionsUpdated OnTrialMissionsUpdated;

	FOnLocalGameplayStarted OnLocalGameplayStartedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnStrongholdStarted OnStrongholdStarted;

	void OnLevelGenerated(const FLevelSettings& levelSettings);

	bool IsInitialisationComplete() const;

	virtual bool IsIngame() { return true; }

	void ForceCompleteCurrentObjective();

	Terrain* GetTerrain() const { return mTerrain.get(); };

	void TerrainRevealAround(TWeakObjectPtr< APlayerCharacter> pChar, const TerrainPos& tPos, float radiusScale, const TerrainPos& delta = TerrainPos(0, 0));

	static std::function< void(AMobCharacter*) >			default_PostSpawnFunc;
	static std::function< void(TArray<AMobCharacter*>&) >	default_PostGroupSpawnFunc;

	void RequestMobSpawn(EntityType, const game::mobspawn::TransformProvider& transformProvider, const TArray<FEnchantmentData>& enchantmentData, const game::mobspawn::Config& config, std::function< void(AMobCharacter*) > inPostSpawnFunc = default_PostSpawnFunc, game::mobspawn::MobSpawnPriority SpawnPriority = game::mobspawn::MobSpawnPriority::ESpawnPriority_Medium);
	void RequestMobSpawn(EntityType, const FTransform& inWorldTransform, const TArray<FEnchantmentData>& enchantmentData, const  game::mobspawn::Config& inMobSpawnConfig, std::function< void(AMobCharacter*) > inPostSpawnFunc = default_PostSpawnFunc, game::mobspawn::MobSpawnPriority SpawnPriority = game::mobspawn::MobSpawnPriority::ESpawnPriority_Medium);
	void RequestMobGroupSpawn(const game::mobspawn::SpawnGroup& mobGroup, const game::mobspawn::TransformProvider& transformProvider, const game::mobspawn::Config& inMobSpawnConfig, std::function< void(TArray<AMobCharacter*>&) > inPostGroupSpawnFunc = default_PostGroupSpawnFunc, std::function< void(AMobCharacter*) > inPostIndividualSpawnFunc = default_PostSpawnFunc, game::mobspawn::MobSpawnPriority SpawnPriority = game::mobspawn::MobSpawnPriority::ESpawnPriority_Medium);
	void RequestCharacterDestroy(ABaseCharacter* pTarget);

	UFUNCTION(BlueprintCallable, Category = "Default")
	void RingAtObjective();

	UPROPERTY()
	bool bUsesObjectives = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeons")
	UMissionPresentationHandler* mPresentationHandler;

	void StartGameLocally(bool playIntro);
	bool IsBeginningOfGame_ServerOnly() const;

	void NotifyWorldMeshesCreated();

	bool HasPendingMobSpawns() const;

	bool CanSpawnMobs() const;

	bool IsReadyForPlayers() const;
	void SetReadyForPlayers_ServerOnly();

	bool IsStarted() const;
	void StartGame_ServerOnly();

	void IncrementRaidDifficulty();
	void AddMidGameAffector();

	void OnAffectorsChanged();
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnAffectorsChanged OnAffectorsChangedDelegate;
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnDifficultyIncreased OnDifficultyIncreased;

	const TArray<TSubclassOf<UBlockTrigger>>& GetBlockTriggers() const { return BlockTriggers; };

	UMobLoaderComponent* GetMobLoader() const { return MobLoader; }

	TArray<FReplicatableModifier> GetGameModifiers();

	FEmergentDifficulty GetReplicatedEmergentDifficulty();

	UFUNCTION(BlueprintPure)
	FText GetCurrentMissionNightName();
	UFUNCTION(BlueprintPure)
	FText GetCurrentMissionNightAttackName();

protected:
	virtual FLevelSettings CreateLevelSettings() const;
	void UpdateDistanceCheckedActors();
	UDungeonsGameInstance* GetDungeonsGameInstance() const;

private:
	void WaitForAsyncTasks();


	void EnqueueGroupSpawn(TSharedPtr<MobGroupSpawnData>, game::mobspawn::MobSpawnPriority);
	void EqueueSpawn(TSharedPtr<MobSpawnData>, game::mobspawn::MobSpawnPriority);

	void StartLevelGeneration(const FLevelSettings&);
	void UpdateInitStage();

	void RemoveOldPhysicsScene();
	void ReInitialisePhysicsScene();

	void OnSuccessfulLevelGeneration();

	void OnLocalGameplayStarted();

	void CheckAndApplyNightMode() const;

	virtual bool HasLocalGameplayStarted();

	void TickServer();
	void UpdateObjectiveLocations();
	void UpdateChallengeRewardLocations();
	void ServerUpdatePartsDiscovered();

	void SetTrialMissionDefsOnServer(const TArray<FReplicatableTrialDef>&);
	void PrepareTrials() const;

	void UpdateMobSpawnRequirements();
	void SetupAdditionalBlockTriggers();

	void OnMissionMobsLoaded();
protected:
	Unique<game::Game> mGame;
private:
	LevelDataStruct mLevelData;
	Unique<Terrain> mTerrain;
	TUniquePtr< FAsyncTask< FTerrainRevealUpdateAsyncTask > > mTerrainUpdateTask;
	TQueue< TerrainRevealUpdateData > mTerrainRevealUpdateQueue;

	TArray< MobSpawnQueue, TFixedAllocator<game::mobspawn::MobSpawnPriority::ESpawnPriority_END > > mMobSpawnQueues;
	TArray< TWeakObjectPtr< ABaseCharacter > > mCharacterDestructionList;
	TArray<TSubclassOf<UBlockTrigger>> BlockTriggers;

	TUniquePtr< FAsyncTask< FLevelGenerationAsyncTask > > mLevelLoadJob;
	TUniquePtr< FAsyncTask< FFadeActorCheckAsyncTask > > mFadeActorUpdateTask;

	bool mHasGameplayStarted = false;
	bool mHasIntroStarted = false;
	bool mHasCreatedMeshes = false;
	bool mCanSpawnMobs = false;

	UPROPERTY(Replicated)
	AGameBPInternal_EServerLoadingState mServerLoadingState = AGameBPInternal_EServerLoadingState::Loading;

	float mServerTimeReadied = 0;

	UPROPERTY(ReplicatedUsing = OnRep_ObjectiveLocations)
	FObjectiveLocations mObjectiveLocations;

	UPROPERTY(Replicated)
	uint8 mObjectiveLocationCalculationType_TEMPCODE;

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_TrialDefs)
	FReplicatableTrialDefPack mTrialDefs;

	UPROPERTY()
	TArray<FAffectorInfo> mActiveAffectorInfo;

	UPROPERTY(ReplicatedUsing=OnRep_AffectorsChanged)
	FReplicatableAffectorsRules mReplicatableAffectorsRules;
	UFUNCTION()
	void OnRep_AffectorsChanged();

	void InitScheduledIntegrityCheck();
	void CheckIntegrity();
	void EnforceFailedIntegrityCheck() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const TArray<FAffectorInfo>& GetActiveAffectorInfo() const;

	UFUNCTION()
	void OnRep_ObjectiveLocations();

	UPROPERTY(ReplicatedUsing = OnRep_ChallengeRewardLocations)
	TArray<FVector> mChallengeRewardLocations;

	UFUNCTION()
	void OnRep_ChallengeRewardLocations();

	UFUNCTION()
	void OnRep_TrialDefs();

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<AObjectiveRing> nextObjectiveActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	int nextObjectiveBlockDistance = 20;

	UPROPERTY(EditInstanceOnly, Category = "Dungeons")
	UMaterialParameterCollection* materialParameterCollection;

	UFUNCTION()
	void OnRep_LevelSettings();
	UFUNCTION()
	void OnRep_ReplicatedEmergentDifficulty();

	UPROPERTY(Transient, ReplicatedUsing=OnRep_LevelSettings)
	FLevelSettings mReplicatedLevelSettings_ONLY_FOR_INITIAL_REPLICATION;
	UPROPERTY(Transient, ReplicatedUsing= OnRep_ReplicatedEmergentDifficulty)
	FEmergentDifficulty mReplicatedEmergentDifficulty;
	FLevelSettings mInitialisingLevelSettings;

	UPROPERTY()
	UMobLoaderComponent* MobLoader = nullptr;

	bool mHasReceivedReplicatedLevelSettings = false;

	int CodeIntegrityCheck;
	int ScheduledCodeIntegrityCheck;
	int ScheduledDebuggerCheck;

	int mCurrentInitState;

	FTimerHandle RecurringIntegrityCheckHandle;

	float mFadeActorTimer;

	bool mHasLoadedMissionMobs = false;
};
