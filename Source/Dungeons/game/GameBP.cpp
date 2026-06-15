#include "Dungeons.h"
#include "GameBP.h"
#include "LobbyBP.h"
#include "DungeonsGameState.h"
#include "DungeonsGameInstance.h"
#include "client/resource/Resource.h"
#include "lovika/LovikaLevelActor.h"
#include "lovika/tile/TilePredicates.h"
#include "lovika/world/level/levelgen/LevelGen.h"
#include "lovika/world/level/levelgen/LevelGenUtil.h"
#include "lovika/world/level/levelgen/LevelValidation.h"
#include "lovika/world/level/levelgen/SourceData.h"
#include "lovika/world/level/levelgen/SourceDataFilter.h"
#include "lovika/world/level/levelgen/TilePlacementFreezer.h"
#include "lovika/world/level/levelgen/hyper/HyperStitcher.h"
#include "lovika/world/level/levelgen/metascore/MetaScoreTypes.h"
#include "lovika/world/level/postprocess/PostProcess.h"
#include "lovika/world/level/postprocess/OutsideFiller.h"
#include "Game.h"
#include "GameProgress.h"
#include "game/Conversion.h"
#include "game/objective/ChallengeSystem.h"
#include "game/objective/ObjectivesSystem.h"
#include "game/objective/Objective.h"
#include "game/actor/item/ItemDropChanceActor.h"
#include "game/difficulty/Difficulty.h"
#include "game/item/ItemTypeDefs.h"
#include "game/item/drop/ItemDropGenerator.h"
#include "game/level/ambience/AmbienceUtil.h"
#include "game/level/sound/AudioSFXManager.h"
#include "ui/MissionPresentationHandler.h"

#include "game/component/drop/EyeOfEnderDefines.h"
#include "game/mobspawn/MobSpawner.h"
#include "game/mission/MissionDefs.h"
#include "game/actor/PropActor.h"
#include "game/mobspawn/MobSpawner.h"
#include "game/mobspawn/MobSpawnConfigs.h"
#include "game/mobspawn/MobSpawnProviders.h"
#include "legacy/Core/Utility/EnumCast.h"
#include "LevelSettings.h"
#include "AbilitySystemGlobals.h"
#include "ui/hints/HintManager.h"
#include "util/ActorQuery.h"
#include "util/StringUtils.h"
#include "util/StringUtil.h"
#include "GameplayCueManager.h"
#include "component/mode/DownedNightModeComponent.h"
#include "online/trials/TrialsProvider.h"
#include <numeric>
#include "Runtime/Engine/Public/Slate/SceneViewport.h"
#include "Assets/PreloadConfig.h"
#include "lovika/world/level/terrain/Pathfinder.h"
#include "util/LocationQuery.h"
#include "online/sessions/OnlineUtil.h"
#include "online/reconnect/ReconnectUtil.h"
#include "FXSystem.h"
#include "abilities/DungeonsGameplayCueManager.h"
#include <Themida/Anticheat.hpp>
#include "component/blocktriggers/WaterBlockTrigger.h"
#include "component/blocktriggers/LavaBlockTrigger.h"
#include "actor/SuddenDeath.h"
#include "difficulty/DifficultyUtil.h"
#include "GameTypes.h"
#include "Engine/AssetManager.h"
#include "affector/Affectors.h"
#include "affector/AffectorTypes.h"
#include "PhysicsEngine/PhysicsSettings.h"
#include "AIController.h"

constexpr int ANTICHEAT_VALID_SCHEDULED_CHECK_VALUE = 0x23f21356;

constexpr int ANTICHEAT_VALID_SCHEDULED_DEBUGGER_CHECK_VALUE = 0x26f2a346;

constexpr int ANTICHEAT_VALID_CHECK_VALUE = 0x55656456;

TAutoConsoleVariable<int32> CVarDebugObjectiveRingPath(
	TEXT("Dungeons.DebugDraw.ObjectiveRingPath"),
	0,
	TEXT("Enables debug drawing of objective ring path.\n")
	TEXT("<= 0: off.\n")
	TEXT(">  0: on.\n"),
	ECVF_Cheat
);

TAutoConsoleVariable<int32> CVarCinematicCullCheck(
	TEXT("Dungeons.CinematicCullCheck"),
	0,
	TEXT("Enables Tick Culling of Props during cinematics\n")
	TEXT("<= 0: off.\n")
	TEXT(">  0: on.\n"),
	ECVF_Cheat
);

DECLARE_SCOPE_CYCLE_COUNTER(TEXT("AGameBP::GenerateLevel"), STAT_GameBP_GenerateLevel, STATGROUP_LoadTime)

namespace game { namespace integrity {
	float getRandomCheckDelayInSeconds() {
		return FMath::FRandRange(200, 400);
	}
}}

TerrainRevealUpdateData::TerrainRevealUpdateData()
	: mRadiusScale(0) {	
}

TerrainRevealUpdateData::TerrainRevealUpdateData(const TWeakObjectPtr<APlayerCharacter> playerChar, const TerrainPos& tPos,
                                                 const float radiusScale, const TerrainPos& delta)
	: mTPos(tPos)
	, mDelta(delta)
	, mRadiusScale(radiusScale)
	, mPlayerChar(playerChar) {
}

MobSpawnData::MobSpawnData(EntityType inMobType, UClass* cls, const FTransform& inWorldPos,
	const TArray<FEnchantmentData>& enchantmentData, const game::mobspawn::Config& inMobSpawnConfig,
	std::function<void(AMobCharacter*)> inPostSpawnFunc)
: MobType(inMobType), MobClass(cls), MobEndData(inWorldPos, enchantmentData, inMobSpawnConfig, inPostSpawnFunc) {
}

MobGroupSpawnData::MobGroupSpawnData(EntityType type, TArray<FTransform>&& transforms, const game::mobspawn::Config& config, MobSpawnCallback individual,
	GroupSpawnCallback group, const game::mobspawn::SpawnGroup& spawnGroup)
: MobType(type), MobTransforms(MoveTemp(transforms)), MobSpawnConfig(config), PostIndividualSpawnFunc(individual), PostGroupSpawnFunc(group)
, EnchantmentData(spawnGroup.enchantmentData), GroupLootDropChance(spawnGroup.hasGroupLoot() ? spawnGroup.groupLootDropChance : 0.f), MobSpawnCount(0) {
	
}

enum InitState
{
	EGameBPInit_Start = 0,
	EGameBPInit_BeginLevelGenTask,
	EGameBPInit_WaitLevelGenTask,
	EGameBPInit_WaitForLevelDef,
	EGameBPInit_RemoveOldPhysicsScene,
	EGameBPInit_CreateNewPhysicsScene,
	EGameBPInit_InitTrials,
	EGameBPInit_SetResourcePack,
	EGameBPInit_DisableDropActor,
	EGameBPInit_CreateTerrain,
	EGameBPInit_WaitForGameRequirements,
	EGameBPInit_CreateGame,
	EGameBPInit_GameInitialise,
	EGameBPInit_StartPreloadMobs,
	EGameBPInit_SetupMusicManager,
	EGameBPInit_SetupPlaySession,
	EGameBPInit_SetupAffectors,
	EGameBPInit_InitDifficulty,
	EGameBPInit_PreloadItems,
	EGameBPInit_BindPlayerCharacterEquipment,
	EGameBPInit_End
};

AGameBP::AGameBP()
	: mCurrentInitState(EGameBPInit_Start)
	, mFadeActorTimer(0.0f)
{
	PrimaryActorTick.bCanEverTick = true;
	bAllowTickBeforeBeginPlay = false;

	bAlwaysRelevant = true;
	SetReplicates(true);
	mPresentationHandler = CreateDefaultSubobject<UMissionPresentationHandler>(TEXT("PresentationHandler"));

	mCharacterDestructionList.Reserve(128);
	mMobSpawnQueues.SetNum(game::mobspawn::MobSpawnPriority::ESpawnPriority_END);
	BlockTriggers = { ULavaBlockTrigger::StaticClass(), UWaterBlockTrigger::StaticClass() };

	MobLoader  = CreateDefaultSubobject<UMobLoaderComponent>(TEXT("MobLoader"));
}


AGameBP::~AGameBP() 
{
	WaitForAsyncTasks();
}

void AGameBP::WaitForAsyncTasks()
{
	if (mTerrainUpdateTask) {
		mTerrainUpdateTask->EnsureCompletion();
		mTerrainUpdateTask = nullptr;
	}

	if (mLevelLoadJob) {
		mLevelLoadJob->EnsureCompletion();
		mLevelLoadJob = nullptr;
	}

	if (mFadeActorUpdateTask) {
		mFadeActorUpdateTask->EnsureCompletion();
		mFadeActorUpdateTask = nullptr;
	}
	if (auto world = GetWorld()) {
		if (auto level = actorquery::getFirstActor<ALovikaLevelActor>(world)) {
			level->AbortMeshGeneration();
		}
	}
}

void AGameBP::UpdateMobSpawnRequirements() {
	if (mHasCreatedMeshes) {
		mCanSpawnMobs = true;
	}
}

void AGameBP::SetupAdditionalBlockTriggers()
{
	auto mission = missions::get(mInitialisingLevelSettings.getLevelName());
	BlockTriggers.Append(mission.getAdditionalBlockTriggers());
	for (auto& trigger : mission.getOverrideBlockTriggers())
	{
		if (auto entry = BlockTriggers.FindByPredicate([&](TSubclassOf<UBlockTrigger> v) { return trigger->IsChildOf(v); }))
		{
			*entry = trigger;
		}
	}
}

void AGameBP::OnMissionMobsLoaded() {
	mHasLoadedMissionMobs = true;
}

void AGameBP::Tick(const float deltaSeconds) {
	Super::Tick(deltaSeconds);
	
	if (mCurrentInitState != EGameBPInit_End)
	{
		UpdateInitStage();
		return;
	}


	if (mGame) { // This can be empty if level load failed (e.g. incorrectly formatted json)
		mGame->update();
		TickServer();

		UpdateDistanceCheckedActors();
		UpdateTerrainReveals();
		UpdateFadeActors(deltaSeconds);

		UpdateMobSpawn();
		UpdateMobSpawnRequirements();		
		
		UpdateCharacterDestroy();

		if (auto* pGameInstance = GetDungeonsGameInstance())
		{
			pGameInstance->UpdateRecentlyLeftControllers(deltaSeconds);
		}
				
		if (mHasIntroStarted &&!mHasGameplayStarted && HasLocalGameplayStarted()) {
			mHasGameplayStarted = true;
			OnLocalGameplayStarted();
			analytics::Analytics::GetInstance().FireLevelStarted(mGame.get(), actorquery::getFirstActor<ALovikaLevelActor>(GetWorld())->getGeneratedLevelSettingsOrEmpty());

			if (auto* pGameInstance = GetDungeonsGameInstance())
			{
				pGameInstance->CleanUpGameRenderTargets();
			}
		}
	}
}

bool AGameBP::HasLocalGameplayStarted() {
	const auto gameState = Cast<ADungeonsGameState>(GetWorld()->GetGameState());
	return !gameState->IsCinematicPlaying();
}

void AGameBP::UpdateTerrainReveals() {
	if (mTerrainUpdateTask && mTerrainUpdateTask->IsDone() && !mTerrainRevealUpdateQueue.IsEmpty()) {
		//task finished, but more data to be processed, kick it off again
		mTerrainUpdateTask->StartBackgroundTask();
	}
}

void AGameBP::UpdateMobSpawn() {
	if (!CanSpawnMobs()) {
		return;
	}
	
	UWorld* pWorld = GetWorld();
	auto gameInstance = Cast<UDungeonsGameInstance>(pWorld->GetGameInstance());
	const bool bLoadingScreenActive = gameInstance->IsLoadingScreenActive();
		
	for (auto& MobSpawnQueueVar : mMobSpawnQueues)
	{
		if (MobSpawnQueueVar.ProcessMobSpawnQueue(pWorld, bLoadingScreenActive))
		{
		#if !WITH_EDITOR
			return; //popped one
		#endif
		}
	}
}

void AGameBP::UpdateFadeActors(const float deltaSeconds)
{
	if(!mHasGameplayStarted)
		return;

	if (mFadeActorTimer <= 0.0f)
	{
		if (mFadeActorUpdateTask && mFadeActorUpdateTask->IsDone())
		{
			mFadeActorUpdateTask->EnsureCompletion(); //make absolutely sure

			UWorld* pWorld = GetWorld();

			FFadeActorCheckAsyncTask& CheckTask = mFadeActorUpdateTask->GetTask();

			//update from previous job
			CheckTask.GameThreadProcessActors();

			//setup & launch new job

			auto& PlayerList = InstanceTracker< APlayerCharacter >::GetList(pWorld);

			if (PlayerList.Num())
			{
				CheckTask.mWorld = pWorld;

				const auto playerController = GetWorld()->GetFirstPlayerController();

				if (playerController)
				{
					APlayerCameraManager* PlayerCameraManager = playerController->PlayerCameraManager;
					if (PlayerCameraManager != NULL)
					{
						CheckTask.mCurrentCameraPosition = PlayerCameraManager->GetCameraLocation();
					}

					const auto level = actorquery::getFirstActor<ALovikaLevelActor>(GetWorld());
					CheckTask.mLevelActor = level;

					CheckTask.mCurrentPlayerActors.Reset();

					//setup player character info
					for (auto PlayerChar : PlayerList)
					{
						FadeActorCheckPlayerData PlayerData;
						
						PlayerData.mPlayerChar = PlayerChar;
						PlayerData.mPlayerLocation = PlayerChar->GetActorLocation();

						UCapsuleComponent* pCapsule = PlayerChar->GetCapsuleComponent();
						if (pCapsule)
						{
							PlayerData.mCapsuleRadius = pCapsule->GetUnscaledCapsuleRadius();
							PlayerData.mCapsuleHalfHeight = pCapsule->GetUnscaledCapsuleHalfHeight();
						}
						else
						{
							//just use some default values instead
							PlayerData.mCapsuleRadius = 10.0f;
							PlayerData.mCapsuleHalfHeight = 10.0f;
						}

						CheckTask.mCurrentPlayerActors.Push(PlayerData);
					}
					
					//kick off the task
					mFadeActorUpdateTask->StartBackgroundTask();

					mFadeActorTimer = 0.15f; //TODO config var?
				}				
			}			
		}
	}
	else
	{
		mFadeActorTimer -= deltaSeconds;
	}
}

void AGameBP::UpdateCharacterDestroy()
{
	if (mCharacterDestructionList.Num())
	{
	
		TWeakObjectPtr< ABaseCharacter > FistWeakChar = mCharacterDestructionList[0];

		if (!FistWeakChar.IsValid())
		{
			mCharacterDestructionList.RemoveAtSwap(0);
		}
		else if (FistWeakChar->DissasembleCharacter())
		{
			//finished disassembling this character
			FistWeakChar->Destroy();
			mCharacterDestructionList.RemoveAtSwap(0);
		}
	}
}

bool AGameBP::GetPreviousDoor(const APlayerCharacter* player, FVector& outPosition) const {
	if (mGame) {
		if (auto door = mGame->tiles().getPreviousDoor(*player)) {
			outPosition = conversion::blockCenterXZToUe(door->door.position());
			return true;
		}
	}
	return false;
}

bool AGameBP::GetNextDoor(const APlayerCharacter* player, FVector& outPosition) const {
	if (mGame) {
		if (auto door = mGame->tiles().getNextDoor(*player)) {
			outPosition = conversion::blockCenterXZToUe(door->door.position());
			return true;
		}
	}
	return false;
}

bool AGameBP::GetNextTileDoor(const APlayerCharacter* player, FVector& outPosition) const {
	if (mGame) {
		if (const auto currentTile = mGame->tiles().getTile(*player)) {
			if (auto door = mGame->tiles().getNextDoor(*player)) {
				if(auto doorBackToThis = mGame->tiles().findDoorBetween(door.GetValue().tile, *currentTile)){
					outPosition = conversion::blockCenterXZToUe(doorBackToThis->door.position());
					return true;
				}
			}
		}
	}
	return false;
}

bool AGameBP::GetPreviousTileDoor(const APlayerCharacter* player, FVector& outPosition) const {
	if (mGame) {
		if (const auto currentTile = mGame->tiles().getTile(*player)) {
			if (auto door = mGame->tiles().getPreviousDoor(*player)) {
				if (auto doorBackToThis = mGame->tiles().findDoorBetween(door.GetValue().tile, *currentTile)) {
					outPosition = conversion::blockCenterXZToUe(doorBackToThis->door.position());
					return true;
				}
			}
		}
	}
	return false;
}

bool AGameBP::GetNextDoorIndicatorDoor(const APlayerCharacter* player, FVector& out) const {
	if (mGame) {
		if (const auto currentTile = mGame->tiles().getTile(*player)) {
			if (auto door = mGame->tiles().getNextDoorIndicatorDoor(*currentTile)) {
				out = conversion::blockCenterXZToUe(door->door.position());
				return true;
			}
		}
	}
	return false;
}

bool AGameBP::GetTriggerRegionCenter(FString regionName, FVector& outPosition) const {
	if (mGame) {
		const auto regionNameStr = stringutil::toStdString(regionName);
		if (auto region = mGame->regionFinder().single(regionNameStr)) {
			outPosition = conversion::blockCuboidToUe(region->area()).GetCenter();
			return true;
		}
	}
	return false;
}

ANTICHEAT_NO_OPTIMIZATION_BEGIN
void AGameBP::BeginPlay() {
	ANTICHEAT_VIRT_BEGIN;
	Super::BeginPlay();
	

	auto* gameInstance = GetGameInstance<UDungeonsGameInstance>();
	if (!gameInstance->HasBaseGameEntitlement()) {
		{
			gameInstance->BeginLoadingScreenWithTravel(FLevelSettings(), EMapLoadType::OpenMenu);
		}
		return;
	}

#ifdef USING_ANTICHEAT
	auto* immediateCheckTask = new FAutoDeleteAsyncTask<FCallbackAsyncTask >([=]() {
		ANTICHEAT_VIRT_CHECK_USER_CODE(CodeIntegrityCheck, ANTICHEAT_VALID_CHECK_VALUE);
	});
	immediateCheckTask->StartBackgroundTask();

	FTimerHandle initHandle;
	gameInstance->GetTimerManager().SetTimer(initHandle, this, &AGameBP::InitScheduledIntegrityCheck, game::integrity::getRandomCheckDelayInSeconds(), false);
#else
	CodeIntegrityCheck = ANTICHEAT_VALID_CHECK_VALUE;
	ScheduledCodeIntegrityCheck = ANTICHEAT_VALID_SCHEDULED_CHECK_VALUE;
	ScheduledDebuggerCheck = ANTICHEAT_VALID_SCHEDULED_DEBUGGER_CHECK_VALUE;
#endif
	
#if !WITH_EDITOR
	FXConsoleVariables::bFreezeParticleSimulation = true;
#endif
	
	GEngine->ForceGarbageCollection(true);



	ANTICHEAT_VIRT_END;
}

void AGameBP::InitScheduledIntegrityCheck() {
	ANTICHEAT_VIRT_BEGIN

	auto* scheduledCheckTask = new FAutoDeleteAsyncTask<FCallbackAsyncTask >([=]() {
		ANTICHEAT_VIRT_CHECK_USER_CODE(ScheduledCodeIntegrityCheck, ANTICHEAT_VALID_SCHEDULED_CHECK_VALUE);
	});	
	scheduledCheckTask->StartBackgroundTask();

	auto* scheduledDebuggerCheckTask = new FAutoDeleteAsyncTask<FCallbackAsyncTask >([=]() {
		ANTICHEAT_CHECK_DEBUGGER(ScheduledDebuggerCheck, ANTICHEAT_VALID_SCHEDULED_DEBUGGER_CHECK_VALUE);
	});	
	scheduledDebuggerCheckTask->StartBackgroundTask();

	FTimerHandle checkHandle;
	GetGameInstance()->GetTimerManager().SetTimer(checkHandle, this, &AGameBP::CheckIntegrity, game::integrity::getRandomCheckDelayInSeconds(), false);
	
	ANTICHEAT_VIRT_END
}

void AGameBP::CheckIntegrity() {
	ANTICHEAT_VIRT_BEGIN

	if (ScheduledCodeIntegrityCheck != ANTICHEAT_VALID_SCHEDULED_CHECK_VALUE || ScheduledDebuggerCheck != ANTICHEAT_VALID_SCHEDULED_DEBUGGER_CHECK_VALUE) {
		FTimerHandle enforceHandle;
		GetGameInstance()->GetTimerManager().SetTimer(enforceHandle, this, &AGameBP::EnforceFailedIntegrityCheck, game::integrity::getRandomCheckDelayInSeconds(), false);		
	}

	ScheduledCodeIntegrityCheck = 0;
	ScheduledDebuggerCheck = 0;

	ANTICHEAT_VIRT_END
}

void AGameBP::EnforceFailedIntegrityCheck() const {
	ANTICHEAT_VIRT_BEGIN
	GetDungeonsGameInstance()->FailedIntegrityCheck();
	ANTICHEAT_VIRT_END
}
ANTICHEAT_NO_OPTIMIZATION_END

FLevelSettings AGameBP::CreateLevelSettings() const {
	check(HasAuthority() && "Level settings should always be created by the authoriative part.");
	auto levelSettings = GetDungeonsGameInstance()->Configuration.GetLevelSettings();
	if (!levelSettings.IsSet()) {
		//No level settings configuration - use whatever has been configured in the lovika level actor development binary data
		// (Usually used during development...) 
		const auto level = actorquery::getFirstActor<ALovikaLevelActor>(GetWorld());
		const auto gameState = Cast<ADungeonsGameState>(GetWorld()->GetGameState());
		const auto seed = level->developmentUseFixedSeed ? level->developmentFixedLevelSeed : (gameState ? gameState->DevelopmentLevelSeed : 0);
		UE_LOG(LogDungeons, Log, TEXT("Generating level using seed: %i"), seed);
		levelSettings = levelsettingsutil::generateDevelopmentMissionSettings(GetWorld(), 
			level->developmentLevelId,
			level->developmentLevelFilename,
			level->developmentDifficulty,
			level->developmentThreatLevel,
			level->developmentEndlessStruggle,
			level->developmentEmergentDifficulty,
			seed
		);
	}
	return levelSettings.GetValue();
}

void AGameBP::StartLevelGeneration(const FLevelSettings& levelSettings) {
	if (HasAuthority()) {
		mReplicatedLevelSettings_ONLY_FOR_INITIAL_REPLICATION = levelSettings;
	}

	mCurrentInitState = EGameBPInit_BeginLevelGenTask;
	mInitialisingLevelSettings = levelSettings;
	mHasCreatedMeshes = false;

	//launch generation job
	if (mLevelLoadJob)
	{
		//make sure any previous ones are completed
		mLevelLoadJob->EnsureCompletion();
		mLevelLoadJob = nullptr;
	}

	mLevelLoadJob = MakeUnique< FAsyncTask< FLevelGenerationAsyncTask > >(mLevelData, mInitialisingLevelSettings);
	mLevelLoadJob->StartBackgroundTask();
}

void AGameBP::EndPlay(EEndPlayReason::Type EndPlayReason) {
	ANTICHEAT_VIRT_BEGIN;

	WaitForAsyncTasks();

	if (mGame) {		
		analytics::Analytics::GetInstance().FireLevelEnded(EndPlayReason);
	}

#ifdef USING_ANTICHEAT
	// D11.Fred : Looks like this can get called before AGameBP::BeginPlay() ( doing trials right at the start of the game )
	// CodeIntegrityCheck = 0 because it hasn't been setup by AGameBP::BeginPlay ( which gets called shortly after this function )
	// It needs someone to take a look who knows more about anti-cheat
	if (CodeIntegrityCheck != ANTICHEAT_VALID_CHECK_VALUE) {
		GetDungeonsGameInstance()->FailedIntegrityCheck();
	}

	CodeIntegrityCheck = 0;
#endif
	Super::EndPlay(EndPlayReason);

	ANTICHEAT_VIRT_END;
}

void AGameBP::PostRegisterAllComponents() {
	Super::PostRegisterAllComponents();
	InstanceTracker< AGameBP >::AddInstance(GetWorld(), this);
}

void AGameBP::PostUnregisterAllComponents() {
	Super::PostUnregisterAllComponents();
	InstanceTracker< AGameBP >::RemoveInstance(GetWorld(), this);
}

UDungeonsGameInstance* AGameBP::GetDungeonsGameInstance() const {
	return Cast<UDungeonsGameInstance>(GetGameInstance());
}

bool AGameBP::HasPendingMobSpawns() const {

	for (auto& MobSpawnQueueVar : mMobSpawnQueues)
	{
		if (MobSpawnQueueVar.AnyPendingSpawns()) { return true; }
	}

	return false;
}

void AGameBP::UpdateInitStage()
{
	switch (mCurrentInitState)
	{
	case EGameBPInit_Start:
	{
		if (HasActorBegunPlay())
		{
			if (auto* pGameInstance = GetDungeonsGameInstance())
			{
				pGameInstance->CleanUpGameRenderTargets();
			}
			
			//Do initial Garbage Collect
			GEngine->ForceGarbageCollection(true);
			GEngine->ConditionalCollectGarbage();
			++mCurrentInitState;
		}
		break;
	}

	case EGameBPInit_BeginLevelGenTask:
	{
		//start the level Gen
		if (HasAuthority())
		{
			StartLevelGeneration(CreateLevelSettings());
			++mCurrentInitState;
		}
		else if (mHasReceivedReplicatedLevelSettings)
		{
			StartLevelGeneration(mReplicatedLevelSettings_ONLY_FOR_INITIAL_REPLICATION);
			++mCurrentInitState;
		}
		break;
	}

	case EGameBPInit_WaitLevelGenTask:
	{
		//wait for the level gen job to be completed
		if (mLevelLoadJob && mLevelLoadJob->IsWorkDone())
		{
			mLevelLoadJob->EnsureCompletion();
			mLevelLoadJob = nullptr;
			OnSuccessfulLevelGeneration();
			SetupAdditionalBlockTriggers();
			++mCurrentInitState;
		}
		break;
	}

	case EGameBPInit_WaitForLevelDef:
	{
		// We're waiting until we have successfully generated a level.
		// Some later steps will crash if this isn't set anyway.
		if (mLevelData.mLevelDef) {
			++mCurrentInitState;
		}

		break;
	}

	case EGameBPInit_RemoveOldPhysicsScene:
	{
		RemoveOldPhysicsScene();
		++mCurrentInitState;
		break;
	}

	case EGameBPInit_CreateNewPhysicsScene:
	{
		ReInitialisePhysicsScene();
		++mCurrentInitState;
		break;
	}

	case EGameBPInit_InitTrials:
	{
		InitTrials();
		++mCurrentInitState;
		break;
	}
	case EGameBPInit_SetResourcePack:
	{
		++mCurrentInitState;
		break;
	}
	case EGameBPInit_DisableDropActor:
	{
		if (!missions::get(mInitialisingLevelSettings.getLevelName()).areDropsEnabled()) {
			auto* ItemDropChanceActor = actorquery::getFirstActor<AItemDropChanceActor>(GetWorld());
			ItemDropChanceActor->Disable();
		}
		++mCurrentInitState;
		break;
	}
	case EGameBPInit_CreateTerrain:
	{
		mTerrain = make_unique<Terrain>();
		mTerrain->clear();
		mTerrain->fill(game::Tiles(*mLevelData.mLevelDef), BlockSource());
		mTerrain->fillDoors(mLevelData.mFilledDoors);
		mTerrainUpdateTask = MakeUnique< FAsyncTask< FTerrainRevealUpdateAsyncTask > >(mTerrain.get(), mTerrainRevealUpdateQueue);

		++mCurrentInitState;
		break;
	}
	case EGameBPInit_WaitForGameRequirements:
	{
		const bool isWaitingForTrials = UMissionDefs::IsTrial(mInitialisingLevelSettings.getLevelName()) && !mTrialDefs.IsSet();
		const bool hasFinished = !isWaitingForTrials;

		if (hasFinished) {
			++mCurrentInitState;
		}
		break;
	}
	case EGameBPInit_CreateGame:
	{
		mGame = std::make_unique<game::Game>(
			*GetWorld(),
			*mLevelData.mLevelDef,
			mInitialisingLevelSettings.missionState,
			HasAuthority());

		//create task for fade mesh updates
		mFadeActorUpdateTask = MakeUnique< FAsyncTask< FFadeActorCheckAsyncTask > >();

		++mCurrentInitState;

		OnGameSettingsInitiallized.Broadcast();
		break;
	}

	case EGameBPInit_GameInitialise:
	{
		if (mGame->Init())
		{
			++mCurrentInitState;

		}
		break;
	}

	case EGameBPInit_StartPreloadMobs:
	{
		auto handle = MobLoader->AsyncLoadMobs(MobLoader->ExtractMissionMobs(), FStreamableDelegate::CreateUObject(this, &AGameBP::OnMissionMobsLoaded));
		//If we failed to create a loading handle for whatever reason, we set loaded to true.
		if(!handle.IsValid()) {
			mHasLoadedMissionMobs = true;
		}

		AAIController::bAIIgnorePlayers = false;

		++mCurrentInitState;
	}

	case EGameBPInit_SetupMusicManager:
	{
		auto* gameInstance = GetDungeonsGameInstance();
		auto* musicManager = gameInstance->GetAudioMusicManager();

		if (musicManager->GetCurrentLevelAudioCollection().AsyncLoadLevelAudio()) //async load level audio
		{
			musicManager->ClearAllTracks();
			musicManager->PushMusicTrack(musicManager->GetCurrentLevelAudioCollection().LevelBackgroundMusic.Get(), EDungeonsMusicPriority::BackgroundMusic);
			gameInstance->GetSoundMixManager()->PushReverb(musicManager->GetCurrentLevelAudioCollection().MainReverb, EDungeonsMusicPriority::BackgroundMusic);
			if (auto* sfxManager = gameInstance->GetAudioSFXManager())
				sfxManager->PushMusicTrack(musicManager->GetCurrentLevelAudioCollection().EnvironmentSFX.Get(), EDungeonsMusicPriority::BackgroundMusic);

			++mCurrentInitState;
		}

		break;
	}
	case EGameBPInit_SetupPlaySession:
	{
		SetupPlaySession();
		++mCurrentInitState;
		break;
	}
	case EGameBPInit_SetupAffectors:
	{
		SetupAffectors();
		++mCurrentInitState;
		break;
	}
	case EGameBPInit_InitDifficulty:
	{
		if (auto* gameMode = actorquery::getFirstActor<ADungeonsGameMode>(GetWorld())) {
			gameMode->InitDifficulty(mInitialisingLevelSettings);
		}

		++mCurrentInitState;
		break;
	}

	case EGameBPInit_PreloadItems:
	{
		//Added here since we might be adding and editing cues while running in editor...
#ifdef WITH_EDITOR
		if (auto cueManager = Cast<UDungeonsGameplayCueManager>(UAbilitySystemGlobals::Get().GetGameplayCueManager())) {
			cueManager->RefreshExecutionAccelerationSet();
		}
#endif

		if(mHasLoadedMissionMobs) {
			if (auto* pGameInstance = GetDungeonsGameInstance())
			{
				pGameInstance->CleanUpGameRenderTargets();
			}
			++mCurrentInitState;
		}
		break;
	}

	case EGameBPInit_BindPlayerCharacterEquipment:
	{
		//bind equipment items for player characters
		for (APlayerCharacter* pChar : InstanceTracker<APlayerCharacter>::GetList(GetWorld()))
		{
			pChar->BindEquipmentSlots();
		}

		++mCurrentInitState;
		break;
	}

	default:break;
	}

}

void AGameBP::RemoveOldPhysicsScene()
{

	UWorld* MyWorld = GetWorld();

	//Destroy Physics states before replacement
	if (auto* pysxScene = MyWorld->GetPhysicsScene())
	{
		pysxScene->WaitPhysScenes();

		TArray<AActor*> AllActors;
		UGameplayStatics::GetAllActorsOfClass(MyWorld, AActor::StaticClass(), AllActors);
		for (AActor* actor : AllActors)
		{
			if (actor)
			{
				TArray<UActorComponent*> Components;
				actor->GetComponents(Components, true);
				if (Components.Num())
				{
					for (UActorComponent* Component : Components)
					{
						if (Component->IsPhysicsStateCreated())
						{
							//D11.SC make sure the deferred flag is cleared so we dont try to defer update with the wrong PhysX Scene
							if (USkeletalMeshComponent* pSkelMeshComp = Cast<USkeletalMeshComponent>(Component))
								pSkelMeshComp->bDeferredKinematicUpdate = false;

							Component->DestroyPhysicsState();
						}
					}
				}
			}
		}
	}

	//recreate Physx Scene with new world bounds
	MyWorld->SetPhysicsScene(nullptr);
}

void AGameBP::ReInitialisePhysicsScene()
{
	if (mLevelData.mLevelDef)
	{
		game::Tiles GameTiles(*mLevelData.mLevelDef);
		BlockCuboid TotalLvlBounds;
		for (auto subdungeon : GameTiles.getSubDungeonInfos())
		{
			//D11.SC Dungeon subbounds need to be oversized to take into account the boundry fill, as this does not get included in the tiles bounds
			BlockPos GrowSize(postprocess::worldfill::NumBorderBlocks, 0, postprocess::worldfill::NumBorderBlocks);
			BlockCuboid subdungeonBounds = growed(subdungeon.bounds, GrowSize);
			TotalLvlBounds.expand(subdungeonBounds);
		}
		FPlatformMisc::LowLevelOutputDebugStringf(TEXT("TotalLvlBounds: %s \n"), UTF8_TO_TCHAR(TotalLvlBounds.toString().c_str()));
		
		UWorld* MyWorld = GetWorld();
		AWorldSettings* WorldSettings = MyWorld->GetWorldSettings(true);

		WorldSettings->bOverrideDefaultBroadphaseSettings = true;
		FBroadphaseSettings& BroadphaseSettings = WorldSettings->BroadphaseSettings;

		BroadphaseSettings = UPhysicsSettings::Get()->DefaultBroadphaseSettings;

		const int iBlockBorder = 20;
		const float UEBlockBorder = iBlockBorder * Math::PE_TO_UE_UNITS;

		//add Boundry blocks each way
		TotalLvlBounds.minInclusive.move(-iBlockBorder, -iBlockBorder, -iBlockBorder);
		TotalLvlBounds.maxExclusive.move(iBlockBorder, iBlockBorder, iBlockBorder);

		BroadphaseSettings.MBPBounds.Min = conversion::blockToUe(TotalLvlBounds.minInclusive);
		BroadphaseSettings.MBPBounds.Max = conversion::blockToUe(TotalLvlBounds.maxExclusive);

		int iStartPointsFound = 0;
		
		//Pull All actor components for any actors already in the world
		TArray<UActorComponent*> ActorComponents;
		{
			auto& PlayerCharacterList = InstanceTracker<APlayerCharacter>::GetList(MyWorld);

			TArray<AActor*> AllActors;
			UGameplayStatics::GetAllActorsOfClass(MyWorld, AActor::StaticClass(), AllActors);
			for (AActor* actor : AllActors)
			{
				if (actor)
				{
					if (actor->IsA<APlayerStart>()) //make sure our player starts are now within the MBR collision area
					{
						const FVector MinPos = BroadphaseSettings.MBPBounds.Min + FVector(UEBlockBorder, UEBlockBorder, UEBlockBorder + (UEBlockBorder * iStartPointsFound));
						const FVector MaxPos = BroadphaseSettings.MBPBounds.Max - FVector(UEBlockBorder, UEBlockBorder, UEBlockBorder + (UEBlockBorder * iStartPointsFound));

						FVector ActorPos = actor->GetActorLocation();
						FVector ActorPosNew = ActorPos.BoundToBox(MinPos, MaxPos);
						actor->SetActorLocation(ActorPosNew);

						//any player characters already spawned on the old spawn points need moving too
						for (auto* pChar : PlayerCharacterList)
						{
							if (pChar)
							{
								const FVector CharPos = pChar->GetActorLocation();
								if (CharPos.Equals(ActorPos))
								{
									pChar->SetActorLocation(ActorPosNew);
								}
							}
						}

						++iStartPointsFound;

					}
					else if (actor->ActorHasTag("BP_3DView")) //Move the 3d view used for inventory into a physics applicable area
					{
						actor->SetActorLocation(FVector(UEBlockBorder, UEBlockBorder, UEBlockBorder * -0.5f));
					}
					else if (actor->ActorHasTag("BelowKillZCatcher")) //lets put this at the bottom of our level bounds
					{
						FVector ZoneSize = (BroadphaseSettings.MBPBounds.Max - BroadphaseSettings.MBPBounds.Min);
						actor->SetActorLocation(FVector(ZoneSize.X*0.5f, ZoneSize.Y*0.5f, (-UEBlockBorder) + 2.0f));
						actor->SetActorScale3D(FVector(ZoneSize.X - 2.0f, ZoneSize.Y - 2.0f, 1.0f));
					}
					
					if(!actor->IsChildActor()) //only add from parent actors to maintain Physics recreation order
					{
						TArray<UActorComponent*> Components;
						actor->GetComponents(Components, true);
						if (Components.Num())
						{
							ActorComponents.Reserve(ActorComponents.Num() + Components.Num());
							ActorComponents += Components;
						}
					}
				}
			}
		}

		//recreate Physx Scene with new world bounds
		MyWorld->CreatePhysicsScene(WorldSettings);		

		//recreate components physics state with new scene
		if (auto* pysxScene = MyWorld->GetPhysicsScene())
		{
			for (UActorComponent* Component : ActorComponents)
			{
				Component->RecreatePhysicsState();
				if (USkeletalMeshComponent* pSkelMeshComp = Cast<USkeletalMeshComponent>(Component))
					pSkelMeshComp->bDeferredKinematicUpdate = false;
			}
		}
	}
}

void AGameBP::OnLocalGameplayStarted() {
	auto* gameInstance = GetDungeonsGameInstance();
	gameInstance->GetHintManager()->LevelLoaded(*mGame);
	if (const auto pc = gameInstance->GetFirstLocalPlayerController()) {
		reconnect::completeReconnect(pc);
	}

	//Permanent night should not start until gameplay starts, as this causes players to die in the intro sequence...
	CheckAndApplyNightMode();

	if (auto gi = GetDungeonsGameInstance()) {
		if (gi->GetLevelSettingsLastStarted().getLevelName() == ELevelNames::thestronghold)
			OnStrongholdStarted.Broadcast(TOTAL_EYES_OF_ENDER);
	}
	OnLocalGameplayStartedDelegate.Broadcast();
}

void AGameBP::CheckAndApplyNightMode() const{
	if (mGame->affectors().IsNightMode()) {
		auto world = GetWorld();
		if (const auto* gm = actorquery::getFirstActor<ADungeonsGameMode>(world)) {
			if (auto* dc = gm->FindComponentByClass<UDownedNightModeComponent>()) {
				dc->EnterPermaNight();
			}
		}

		if (auto* sd = actorquery::getFirstActor<ASuddenDeath>(world)) {
			sd->SetMode(ESuddenDeathMode::OnAnyPlayerDowned);
		}
	}
}

void AGameBP::InitTrials() {
	if (HasAuthority()) {
		auto trialsProvider = GetDungeonsGameInstance()->GetMinecraftAPI()->GetTrialsProvider();
		mTrialDefs = trialsProvider->GetCachedTrials();
		mTrialDefs.MarkSet();

		if (!IsIngame()) { // If we're in Ingame.uasset, we don't want to accidentally get new trials mid game
			trialsProvider->OnDailyTrialsProvided.AddUObject(this, &AGameBP::SetTrialMissionDefsOnServer);
		}
	}
	PrepareTrials();
}

void AGameBP::TerrainRevealAround(TWeakObjectPtr<APlayerCharacter> pChar, const TerrainPos& tPos, float radiusScale, const TerrainPos& delta /*= TerrainPos(0, 0)*/)
{
	mTerrainRevealUpdateQueue.Enqueue(TerrainRevealUpdateData(pChar, tPos, radiusScale, delta));
}

std::function< void(AMobCharacter*) >			AGameBP::default_PostSpawnFunc = [](AMobCharacter* pMob) {};
std::function< void(TArray<AMobCharacter*>&) >	AGameBP::default_PostGroupSpawnFunc = [](TArray<AMobCharacter*>& pMob) {};

void AGameBP::RequestMobSpawn(EntityType inMobType, const FTransform& inWorldTransform, const TArray<FEnchantmentData>& enchantmentData, const  game::mobspawn::Config& inMobSpawnConfig, std::function< void(AMobCharacter*) > inPostSpawnFunc, game::mobspawn::MobSpawnPriority SpawnPriority)
{
	TSharedPtr<MobSpawnData> mobspawn = MakeShared<MobSpawnData>(inMobType, MobLoader->GetClass(inMobType), inWorldTransform, enchantmentData, inMobSpawnConfig,inPostSpawnFunc);

	if(!mobspawn->MobClass) {
		mobspawn->LoadRequest = MobLoader->AsyncLoadMobs({inMobType}, FStreamableDelegate::CreateUObject(this, &AGameBP::EqueueSpawn, mobspawn, SpawnPriority));

		//If the load request fails due to no mapped asset we need to catch that and do an empty spawn.
		if(!mobspawn->LoadRequest.IsValid()) {
			EqueueSpawn(mobspawn, SpawnPriority);
		}

	} else {
		EqueueSpawn(mobspawn, SpawnPriority);
	}
}


void AGameBP::RequestMobSpawn(EntityType inMobType, const game::mobspawn::TransformProvider& transformProvider, const TArray<FEnchantmentData>& enchantmentData, const game::mobspawn::Config& config, std::function< void(AMobCharacter*) > inPostSpawnFunc, game::mobspawn::MobSpawnPriority SpawnPriority)
{
	FTransform MobTransform;

	if (game::mobspawn::getSpawnData(*GetWorld(), transformProvider, config, inMobType, MobTransform)) {
		RequestMobSpawn(inMobType, MobTransform, enchantmentData, config, inPostSpawnFunc, SpawnPriority);
	} else {
		inPostSpawnFunc(nullptr);
	}
}

void AGameBP::RequestMobGroupSpawn(const  game::mobspawn::SpawnGroup& mobGroup, const  game::mobspawn::TransformProvider& transformProvider, const game::mobspawn::Config& inMobSpawnConfig, std::function< void(TArray<AMobCharacter*>&) > inPostGroupSpawnFunc , std::function< void(AMobCharacter*) > inPostIndividualSpawnFunc, game::mobspawn::MobSpawnPriority SpawnPriority)
{
	UE_LOG(LogDungeons, Log, TEXT("MobSpawn:: AGameBP::RequestMobGroupSpawn: %s, %i"), *FString(EntityTypeToString(mobGroup.type).c_str()), mobGroup.count);
	auto& ThisWorld = *GetWorld();

	EntityType entityType;
	TArray< FTransform > MobTransforms;

	if (!game::mobspawn::getGroupSpawnData(ThisWorld, mobGroup, transformProvider, inMobSpawnConfig, entityType, MobTransforms))
	{
		//do post func with null spawn for each
		for (int i(0); i < mobGroup.count; ++i)
		{
			inPostIndividualSpawnFunc(nullptr);
		}

		//failed to get spawn data
		TArray<AMobCharacter*> BlankList;
		inPostGroupSpawnFunc(BlankList);

		return;
	}


	int iMissingMobs = mobGroup.count - MobTransforms.Num();
	//do post func with null spawn for each missing mob
	for (int i(0); i < iMissingMobs; ++i)
	{
		inPostIndividualSpawnFunc(nullptr);
	}

	if (MobTransforms.Num() == 0)
	{
		//no transforms to be had
		TArray<AMobCharacter*> BlankList;
		inPostGroupSpawnFunc(BlankList);
	}
	else
	{
		TSharedPtr< MobGroupSpawnData > pSpawnGroupData = MakeShared< MobGroupSpawnData >(entityType, MoveTemp(MobTransforms), inMobSpawnConfig, inPostIndividualSpawnFunc, inPostGroupSpawnFunc,  mobGroup);

		if(MobLoader->HasLoadedMob(entityType)) {
			EnqueueGroupSpawn(pSpawnGroupData, SpawnPriority);
		} else {
			pSpawnGroupData->LoadRequest = MobLoader->AsyncLoadMobs({entityType}, FStreamableDelegate::CreateUObject(this, &AGameBP::EnqueueGroupSpawn, pSpawnGroupData, SpawnPriority));

			//If the entity type could not be resolved to a valid asset we get a null handle back and need to signal spawn failiure
			if(!pSpawnGroupData) {
				for (int i(0); i < pSpawnGroupData->MobTransforms.Num(); ++i) {
					inPostIndividualSpawnFunc(nullptr);
				}

				TArray<AMobCharacter*> BlankList;
				inPostGroupSpawnFunc(BlankList);
			}
		}
	}
}

void AGameBP::EnqueueGroupSpawn(TSharedPtr<MobGroupSpawnData> data, game::mobspawn::MobSpawnPriority SpawnPriority) {
	for (auto& TransformPos : data->MobTransforms)
	{
		//push this spawn into request
		data->MobSpawnCount++;

		RequestMobSpawn(data->MobType, TransformPos, data->EnchantmentData, data->MobSpawnConfig, [data](AMobCharacter* pMob) {

			if (pMob)
			{
				data->MobList.Push(pMob);
			}

			data->PostIndividualSpawnFunc(pMob);

			data->MobSpawnCount--;

			//last mob in the spawn list, group finished spawning
			if (data->MobSpawnCount == 0)
			{
				// This list includes <nullptr> if the mob has been garbage collected
				auto allSpawnedMobs = algo::map_tarray(data->MobList, RETLAMBDA(it.Get()));

				if (data->GroupLootDropChance > 0.f) {
					game::mobspawn::applyGroupLoot(allSpawnedMobs, data->GroupLootDropChance);
				}

				data->PostGroupSpawnFunc(allSpawnedMobs);
			}
		}, SpawnPriority);

	}
}

void AGameBP::EqueueSpawn(TSharedPtr<MobSpawnData> mobspawn, game::mobspawn::MobSpawnPriority SpawnPriority) {
	if(!MobLoader->HasLoadedMob(mobspawn->MobType)) {
		mobspawn->MobEndData.PostSpawnFunc(nullptr);
	} else {
		mobspawn->MobClass = MobLoader->GetClass(mobspawn->MobType);
		mMobSpawnQueues[SpawnPriority].mMobSpawnQueue.Enqueue(mobspawn);
		UE_LOG(LogDungeons, Log, TEXT("MobSpawn:: AGameBP::EqueueSpawn: %s"), *FString(EntityTypeToString(mobspawn->MobType).c_str()));
	}
}

void AGameBP::RequestCharacterDestroy(ABaseCharacter* pTarget)
{
	if (pTarget)
	{
		TWeakObjectPtr< ABaseCharacter > WeakChar = pTarget;
		mCharacterDestructionList.Push(WeakChar);
	}
}

void AGameBP::SetupPlaySession() {
	auto level = actorquery::getFirstActor<ALovikaLevelActor>(GetWorld());

	const auto resourcePacks = algo::copy_if_map_as<std::vector<ResourcePack>>(mGame->tiles().getSubDungeonInfos(),
		RETLAMBDA(it.dungeon.resourcePack()),
		RETLAMBDA(ResourcePack(it.dungeon.resourcePack().GetValue()))
	);

	level->setupGameAndBuildMesh(*mGame, *mLevelData.mRegion, resourcePacks);

	const FAmbienceIDGroup defaultAmbience{ EAmbienceID::AMBIENCE_DEFAULT, stringutil::toFString(mLevelData.mLevelDef->levelDef.id) };
	game::ambience::AmbienceUtil::SetActiveAmbienceT<AAmbienceVisualActor>(GetWorld(), defaultAmbience);
}


void AGameBP::SetupAffectors() {
	mActiveAffectorInfo = mGame->affectors().GetActiveAffectorsInfo();
	OnActiveAffectorInfoChanged.Broadcast();
}

void AGameBP::OnRep_AffectorsChanged() {
	OnGameSettingsInitiallized.RemoveDynamic(this, &AGameBP::OnRep_AffectorsChanged);
	if (mGame != nullptr)
	{
		mGame->SetReplicatedAffectors(mReplicatableAffectorsRules);
		SetupAffectors();
		CheckAndApplyNightMode();
	}
	else
	{
		OnGameSettingsInitiallized.AddDynamic(this, &AGameBP::OnRep_AffectorsChanged);
	}
}

TArray<FReplicatableModifier> AGameBP::GetGameModifiers()
{
	return mReplicatableAffectorsRules.modifiers;
}

FEmergentDifficulty AGameBP::GetReplicatedEmergentDifficulty()
{
	return mReplicatedEmergentDifficulty;
}

FText AGameBP::GetCurrentMissionNightName()
{
	if (mGame) {
		return missions::get(mGame->settings().levelName).GetNightName();
	}
	return FText::GetEmpty();
}

FText AGameBP::GetCurrentMissionNightAttackName()
{
	if (mGame) {
		return missions::get(mGame->settings().levelName).GetNightAttackName();
	}
	return FText::GetEmpty();
}

game::Game* AGameBP::GetGame() const {
	return mGame.get();
}

game::LevelDef* AGameBP::GetLevelDef() const {
	return mLevelData.mLevelDef.get();
}

std::string AGameBP::GetCurrentLevelId() const {
	return Util::toLower(mLevelData.mLevelDef->levelDef.id);
}

ChunkBlockSource* AGameBP::BlockSourceWithLoadedMaterials() const {
	if (mCurrentInitState > EGameBPInit_SetupPlaySession) {
		return BlockSource();
	}
	return nullptr;
}

ChunkBlockSource* AGameBP::BlockSource() const {
	return mLevelData.mRegion.get();
}

bool AGameBP::IsPlayerInRangeOfObjective(APlayerCharacter* player) const {
	if (!mObjectiveLocations.bUseLocations) {
		return false;
	}
	if (!GetGame()) {
		return false;
	}
	
	if (const auto tile = GetGame()->tiles().getTile(*player)) {
		const auto tileAt = tile->progress();

		const auto forceObjectiveIndicators = [&]() {
			if (const auto objectives = mGame->objectives()) {
				if (auto current = objectives->current()) {
					return current->info().forceObjectiveIndicator;
				}
			}
			return false;
		}();
		for (auto& objectiveLocation : mObjectiveLocations.Locations) {
			if (const auto objectiveTile = GetGame()->tiles().getTile(conversion::ueToBlock(objectiveLocation))) {
				if (forceObjectiveIndicators && tile->dungeon().instanceId() == objectiveTile->dungeon().instanceId()) {
					return true;
				}

				if (GetObjectiveConsiderTilesType() == io::ObjectiveConsiderTilesType::AlwaysVisible) {
					return true;
				}

				if (GetObjectiveConsiderTilesType() == io::ObjectiveConsiderTilesType::All && objectiveTile->progress().stretches().index() == tileAt.stretches().index()) {
					return true;
				}

				if(GetObjectiveConsiderTilesType() == io::ObjectiveConsiderTilesType::Main && objectiveTile->progress().mostlyEquals(tileAt)) {
					return true;
				}
			}
		}
	}

	return false;
}


void AGameBP::OnSuccessfulLevelGeneration()
{
	GetGameInstance<UDungeonsGameInstance>()->SetLevelSettingsLastStarted(mInitialisingLevelSettings);

	//Remember the successful generation
	auto level = actorquery::getFirstActor<ALovikaLevelActor>(GetWorld());
	level->setGeneratedLevelSettings(mInitialisingLevelSettings);

	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_UEngine_AGameBP_GenerateLevel_OnLevelGenerated);
		OnLevelGenerated(mInitialisingLevelSettings);
	}

	if (auto* gameMode = actorquery::getFirstActor<ADungeonsGameMode>(GetWorld())) {
		QUICK_SCOPE_CYCLE_COUNTER(STAT_UEngine_AGameBP_GenerateLevel_gameModeLevelGenerated);

		gameMode->LevelGenerated(mInitialisingLevelSettings);
	}
}

DECLARE_CYCLE_STAT(TEXT("Update Distance Checked Actors"), STAT_CullDistanceCheck, STATGROUP_Game);
#define MOB_DIST_CHECK 3000.0f //3000.0f (600.0f)
#define MOB_DIST_CHECK_SQ (MOB_DIST_CHECK*MOB_DIST_CHECK)

struct PlayerDistanceCheckValues {
	FVector		PlayerPos;
	float		PlayerArmOffsetRange;
	APlayerCharacter* PlayerPointer;

	PlayerDistanceCheckValues():PlayerPos(0.f, 0.f, 0.f),PlayerArmOffsetRange(1.0f),PlayerPointer(nullptr){};
};

void AGameBP::UpdateDistanceCheckedActors()
{
	SCOPE_CYCLE_COUNTER(STAT_CullDistanceCheck);
	//May be able to spread this out across frames ( or give each a count down)
	auto pWorld = GetWorld();
	auto& PropList = APropActor::Instances.GetList(pWorld);
	auto& MobList = InstanceTracker< AMobCharacter >::GetList(pWorld);
	auto& PlayerList = InstanceTracker< APlayerCharacter >::GetList(pWorld);

	if (PlayerList.Num() && (PropList.Num() || MobList.Num()))
	{

		bool ViewFrustumAvailable = false;
		FSceneViewProjectionData ProjectionData;		
		FConvexVolume FrustumVolume;

		FMatrix ViewProjMatrix;
		{
			ULocalPlayer* LocalPlayer = pWorld->GetFirstLocalPlayerFromController();
			if (LocalPlayer != nullptr && LocalPlayer->ViewportClient != nullptr && LocalPlayer->ViewportClient->Viewport)
			{
				// get the projection data
				if (LocalPlayer->GetProjectionData(LocalPlayer->ViewportClient->Viewport, eSSP_FULL, ProjectionData))
				{
					ViewProjMatrix = ProjectionData.ComputeViewProjectionMatrix();
					//we only care about the 4 outer planes
					FrustumVolume.Planes.Empty(6);
					FPlane	Temp;

					// Left clipping plane.
					if (ViewProjMatrix.GetFrustumLeftPlane(Temp))
					{
						FrustumVolume.Planes.Add(Temp);
					}

					// Right clipping plane.
					if (ViewProjMatrix.GetFrustumRightPlane(Temp))
					{
						FrustumVolume.Planes.Add(Temp);
					}

					// Top clipping plane.
					if (ViewProjMatrix.GetFrustumTopPlane(Temp))
					{
						FrustumVolume.Planes.Add(Temp);
					}

					// Bottom clipping plane.
					if (ViewProjMatrix.GetFrustumBottomPlane(Temp))
					{
						FrustumVolume.Planes.Add(Temp);
					}

					FrustumVolume.Init();

					ViewFrustumAvailable = true;
				}
			}

		}

		const auto gameState = Cast<ADungeonsGameState>(pWorld->GetGameState());

		if (gameState && gameState->IsCinematicPlaying())
		{

			if (ViewFrustumAvailable && CVarCinematicCullCheck.GetValueOnGameThread() > 0)
			{
				for (auto& PropData : PropList)
				{

					const FVector PropLocation = PropData.GetActorLocation();
					const float fCheckDistance = PropData.CheckDistance;
					const float fCheckDistanceSq = fCheckDistance * fCheckDistance;

					bool bVisible = (FVector::DistSquared(ProjectionData.ViewOrigin, PropLocation) < fCheckDistanceSq);


					if (!bVisible)
					{
						//transform the objects position into screen space
						FPlane Result = ViewProjMatrix.TransformFVector4(FVector4(PropLocation, 1.f));
						if (Result.W > 0.0f)
						{
							const float RHW = 1.0f / Result.W;
							const float AbsScreenX = fabs(Result.X * RHW);
							const float AbsScreenY = fabs(Result.Y * RHW);

							if (AbsScreenX < 1.01f && AbsScreenY < 1.01f)
							{
								bVisible = true;
							}
							else if (AbsScreenX < 1.5f && AbsScreenY < 1.5f) //close to the screen bounds
							{
								float BoundsRadius = PropData.Actor->GetRootComponent()->Bounds.SphereRadius;
								//lets frustum check it
								if (FrustumVolume.IntersectSphere(PropLocation, BoundsRadius))
								{
									bVisible = true;
								}
							}
						}
					}

					PropData.SetPlayerVisible(bVisible);
				}
			}
			else
			{
				//Call any that have changed
				for (auto& PropData : PropList)
				{
					if (PropData.DoPlayerDistanceCheck && !PropData.IsPlayerVisible())
					{
						PropData.SetPlayerVisible(true);
					}
				}
			}


			for (auto MobInst : MobList)
			{
				if (MobInst->mPlayerDistanceCheck)
				{
					if (!MobInst->mPlayerVisible)
					{
						MobInst->mPlayerVisible = true;
						MobInst->PlayerEnteredVisibleRange();
					}

					if (MobInst->mExtremeRangeFromPlayers)
					{
						MobInst->mExtremeRangeFromPlayers = false;
						MobInst->MobAtExtremePlayerRangeChanged(false);
					}
				}
			}
		}
		else
		{
			TArray< PlayerDistanceCheckValues, TInlineAllocator< 32 > >	PlayerCheckValues; //need more than 32 players?
			PlayerCheckValues.Reset();
			//Pull the player positions out as GetActorLocation can do a lot of internal work
			for (auto PlayerChar : PlayerList) {
				PlayerDistanceCheckValues CheckVals;
				CheckVals.PlayerPointer = PlayerChar;
				CheckVals.PlayerPos = PlayerChar->GetActorLocation();

				//calculate the current camera distance compared to its base
				if (auto* pCamSpringArm = PlayerChar->GetCameraSpringArm())
				{
					const float fRange = pCamSpringArm->TargetArmLength / pCamSpringArm->SeekArmLength;
					if (fRange > 1.0f)
					{
						CheckVals.PlayerArmOffsetRange = fRange;
					}
				}

				PlayerCheckValues.Push(CheckVals);
			}

			//Props first
			for (auto& PropData : PropList)
			{
				if (PropData.DoPlayerDistanceCheck)
				{
					const FVector PropLocation = PropData.GetActorLocation();
					const float fCheckDistance = PropData.CheckDistance;
					const float fCheckDistanceSq = fCheckDistance * fCheckDistance;
					bool bVisible = false;

					//iInd if the prop is within distance of any player
					for (auto& PlayerCharCheckVals : PlayerCheckValues)
					{ 
						if (FVector::DistSquaredXY(PlayerCharCheckVals.PlayerPos, PropLocation) < (fCheckDistanceSq * PlayerCharCheckVals.PlayerArmOffsetRange * PlayerCharCheckVals.PlayerArmOffsetRange))
						{
							bVisible = true;
							break;
						}
					}

					if (!bVisible && ViewFrustumAvailable)
					{
						//transform the objects position into screen space
						FPlane Result = ViewProjMatrix.TransformFVector4(FVector4(PropLocation, 1.f));
						if (Result.W > 0.0f)
						{
							const float RHW = 1.0f / Result.W;
							const float AbsScreenX = fabs(Result.X * RHW);
							const float AbsScreenY = fabs(Result.Y * RHW);

							if (AbsScreenX < 1.01f && AbsScreenY < 1.01f)
							{
								bVisible = true;
							}
							else if (AbsScreenX < 1.25f && AbsScreenY < 1.25f) //close to the screen bounds
							{
								float BoundsRadius = PropData.Actor->GetRootComponent()->Bounds.SphereRadius;
								//lets frustum check it
								if (FrustumVolume.IntersectSphere(PropLocation, BoundsRadius))
								{
									bVisible = true;
								}
							}
						}
					}

					PropData.SetPlayerVisible(bVisible);
				}
			}

			int iMobsExtreme = 0;

			//Mobs now
			for (auto MobInst : MobList)
			{
				if (MobInst->mPlayerDistanceCheck)
				{
					const FVector MobLocation = MobInst->GetActorLocation();
					MobInst->mPlayersVisible.Reset();
					bool bVisible = false;
					int iExtremeRangeCount = 0;

					//FInd if the prop is within distance of any player
					for (auto& PlayerCharCheckVals : PlayerCheckValues)
					{
						const float DistSq = FVector::DistSquaredXY(PlayerCharCheckVals.PlayerPos, MobLocation);
						const float DistSqCheck = (MOB_DIST_CHECK_SQ * PlayerCharCheckVals.PlayerArmOffsetRange * PlayerCharCheckVals.PlayerArmOffsetRange);
						if (DistSq < DistSqCheck)
						{
							bVisible = true;
							MobInst->mPlayersVisible.Add(PlayerCharCheckVals.PlayerPointer);
						}
						else if (DistSq > (DistSqCheck*2.0f))
						{
							++iExtremeRangeCount;
						}
					}

					//are we > extreme range from all players?
					bool bExtremeRange = (iExtremeRangeCount == PlayerCheckValues.Num());

					//Visibility has changed
					if (MobInst->mPlayerVisible != bVisible)
					{
						MobInst->mPlayerVisible = bVisible;

						if (bVisible)
						{
							MobInst->PlayerEnteredVisibleRange();
						}
						else
						{
							MobInst->PlayerLeftVisibleRange();
						}
					}

					if (bExtremeRange)
					{
						++iMobsExtreme;
					}

					if (MobInst->mExtremeRangeFromPlayers != bExtremeRange)
					{
						MobInst->mExtremeRangeFromPlayers = bExtremeRange;
						MobInst->MobAtExtremePlayerRangeChanged(bExtremeRange);
					}

				}
			}

			//FPlatformMisc::LowLevelOutputDebugStringf(TEXT("##Mobs At extreme Range : %d of %d\n"), iMobsExtreme, MobList.Num());

		}
	}
}

void AGameBP::StartGameLocally(bool playIntro) {
	mHasIntroStarted = true;
	if (playIntro) {
		mGame->StartIntroCinematic();
	}
}

bool AGameBP::IsBeginningOfGame_ServerOnly() const {
	const float SecondsWhenPlayersCanSeeIntro = 60;
	return mServerTimeReadied + SecondsWhenPlayersCanSeeIntro > GetWorld()->GetRealTimeSeconds();
}

void AGameBP::NotifyWorldMeshesCreated()
{
	mHasCreatedMeshes = true;
}

bool AGameBP::CanSpawnMobs() const {
	return mCanSpawnMobs;
}

bool AGameBP::IsReadyForPlayers() const {
	return mServerLoadingState != AGameBPInternal_EServerLoadingState::Loading;
}

void AGameBP::SetReadyForPlayers_ServerOnly() {
	check(HasAuthority() && mServerLoadingState == AGameBPInternal_EServerLoadingState::Loading);
	mServerLoadingState = AGameBPInternal_EServerLoadingState::ReadyForPlayers;
	mServerTimeReadied = GetWorld()->GetRealTimeSeconds();
}

bool AGameBP::IsStarted() const {
	return mServerLoadingState == AGameBPInternal_EServerLoadingState::Started;
}

void AGameBP::StartGame_ServerOnly() {
	check(HasAuthority() && mServerLoadingState == AGameBPInternal_EServerLoadingState::ReadyForPlayers);
	mServerLoadingState = AGameBPInternal_EServerLoadingState::Started;
}

void AGameBP::OnLevelGenerated(const FLevelSettings& levelSettings)
{
	const FMapColors MapColors = UMissionDefs::GetMapColors(levelSettings.getLevelName());

	if (materialParameterCollection)
	{
		if (UWorld* World = GetWorld())
		{
			UMaterialParameterCollectionInstance* Instance = World->GetParameterCollectionInstance(materialParameterCollection);

			{
				const bool bFoundParameter = Instance->SetVectorParameterValue(FName(TEXT("MapColorBase")), MapColors.mBaseColor);

				if (!bFoundParameter) {
					UE_LOG(LogTemp, Warning, TEXT("Could not find VectorParameterValue %s"), TEXT("MapColorBase"));
				}
			}

			{
				const bool bFoundParameter = Instance->SetVectorParameterValue(FName(TEXT("MapColorHigh")), MapColors.mAboveColor);

				if (!bFoundParameter) {
					UE_LOG(LogTemp, Warning, TEXT("Could not find VectorParameterValue %s"), TEXT("MapColorHigh"));
				}
			}

			{
				const bool bFoundParameter = Instance->SetVectorParameterValue(FName(TEXT("MapColorLow")), MapColors.mBelowColor);

				if (!bFoundParameter) {
					UE_LOG(LogTemp, Warning, TEXT("Could not find VectorParameterValue %s"), TEXT("MapColorLow"));
				}
			}
		}
	}

}

bool AGameBP::IsInitialisationComplete() const
{
	return mCurrentInitState == EGameBPInit_End;
}

void AGameBP::SetTrialMissionDefsOnServer(const TArray<FReplicatableTrialDef>& trialDefs) {
	if (!HasAuthority()) {
		return;
	}
	mTrialDefs.Set(trialDefs);
	PrepareTrials();
}

void AGameBP::PrepareTrials() const {
	GetGameInstance<UDungeonsGameInstance>()->SetCurrentTrials(mTrialDefs.ToTrialDefs());
	OnTrialMissionsUpdated.Broadcast();
}

void AGameBP::ForceCompleteCurrentObjective() {
	if (!HasAuthority()) {
		return;
	}
	mGame->forceCompleteCurrentObjective();
	UpdateObjectiveLocations();
}

void AGameBP::UpdateObjectiveLocations() {
	if (!HasAuthority()) {
		return;
	}
	if (const auto objectives = mGame->objectives()) {
		if (auto current = objectives->current()) {
			if (!current->completion().isCompleted()) {
				const auto locations = current->getLocations();
				if (locations != mObjectiveLocations) {
					mObjectiveLocations = locations;
					OnRep_ObjectiveLocations();
				}
				mObjectiveLocationCalculationType_TEMPCODE = enum_cast(current->info().considerTiles.Get(io::ObjectiveConsiderTilesType::Main));
			}
		}
	}
}

void AGameBP::UpdateChallengeRewardLocations() {
	if (!HasAuthority()) {
		return;
	}
	if (const auto challenges = mGame->challenges()) {
		const auto rewardLocations = challenges->getRewardLocations();
		if (rewardLocations != mChallengeRewardLocations) {
			mChallengeRewardLocations = rewardLocations;
			OnRep_ChallengeRewardLocations();
		}
	}
}

void AGameBP::ServerUpdatePartsDiscovered() {
	if (!HasAuthority()) {
		return;
	}
	if (!mGame) {
		return;
	}
	if (auto* gameState = GetWorld()->GetGameState<ADungeonsGameState>()) {
		const int totalPartsDiscovered = [&] {
			const int previousPartsDiscovered = mInitialisingLevelSettings.missionState.partsDiscovered;
			const int sessionPartsDiscovered = mGame->progress().hyperMissionVisitedLevelCount();
			return previousPartsDiscovered + sessionPartsDiscovered;
		}();
		gameState->SetPartsDiscovered(totalPartsDiscovered);
	}
}

void AGameBP::RingAtObjective() {
	const auto world = GetWorld();
	const auto player = actorquery::getFirstActor<APlayerCharacter>(world);
	const auto playerLocation = player->GetActorLocation();

	const auto targetLocation = [&] {
		FVector doorLocation;
		GetNextDoorIndicatorDoor(player, doorLocation);

		FVector closestLocation = doorLocation;
		const auto& locations = GetObjectiveLocations();

		if (locations.bUseLocations && locations.Locations.Num() > 0) {
			float distanceMin = FVector::DistSquared2D(playerLocation, doorLocation);

			for (auto objectiveLocation : locations.Locations) {
				const auto distanceCandidate = FVector::DistSquared2D(playerLocation, objectiveLocation);
				if (distanceCandidate < distanceMin) {
					closestLocation = objectiveLocation;
					distanceMin = distanceCandidate;
				}
			}
		}

		return closestLocation;
	}();

	const auto start = conversion::ueToTerrain(playerLocation);
	const auto end = conversion::ueToTerrain(targetLocation);

	const auto path = pathfinder::trace(*mTerrain, start, end);

	const auto soundWorldLocation = [&]() -> FVector {
		const auto soundTerrainLocation = path.empty() ? end
			: path.size() <= nextObjectiveBlockDistance ? path[0]
			: path[path.size() - nextObjectiveBlockDistance - 1];

		auto location = conversion::terrainToUe(soundTerrainLocation);
		location.Z = playerLocation.Z;

		const auto maybeZ = locationquery::findGround(*world, location, true);
		location.Z = maybeZ.Get(location.Z);

		return location;
	}();

	if (nextObjectiveActorClass != nullptr) {
		if (auto* objectiveRing = world->SpawnActor<AObjectiveRing>(nextObjectiveActorClass, FTransform { playerLocation })) {
			objectiveRing->SetPath(playerLocation, soundWorldLocation);
		}
	}

	if (CVarDebugObjectiveRingPath.GetValueOnGameThread() > 0) {
		DrawDebugLine(
			world,
			soundWorldLocation - FVector{ 0.f, 0.f, 2000.f },
			soundWorldLocation + FVector{ 0.f, 0.f, 2000.f },
			FColor::Red,
			false,
			5.f
		);

		for (const auto& entry : path) {
			auto pointer = conversion::terrainToUe(entry);
			pointer.Z = playerLocation.Z;

			DrawDebugLine(
				world,
				pointer - FVector{ 0.f, 0.f, 1000.f },
				pointer + FVector{ 0.f, 0.f, 1000.f },
				FColor::White,
				false,
				5.f
			);
		}
	}
}

void AGameBP::TickServer() {
	if (!HasAuthority()) {
		return;
	}
	// Tick objective locations. This is synchronized to all clients
	UpdateObjectiveLocations();
	UpdateChallengeRewardLocations();
	ServerUpdatePartsDiscovered();
}

const FObjectiveLocations& AGameBP::GetObjectiveLocations() {
	return mObjectiveLocations;
}

io::ObjectiveConsiderTilesType AGameBP::GetObjectiveConsiderTilesType() const {
	return static_cast<io::ObjectiveConsiderTilesType>(mObjectiveLocationCalculationType_TEMPCODE);
}

const TArray<FVector>& AGameBP::GetChallengeRewardLocations() const {
	return mChallengeRewardLocations;
}

void AGameBP::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGameBP, mObjectiveLocations);
	DOREPLIFETIME(AGameBP, mChallengeRewardLocations);
	DOREPLIFETIME(AGameBP, mObjectiveLocationCalculationType_TEMPCODE);
	DOREPLIFETIME(AGameBP, mTrialDefs);
	DOREPLIFETIME(AGameBP, mReplicatedLevelSettings_ONLY_FOR_INITIAL_REPLICATION);
	DOREPLIFETIME(AGameBP, mServerLoadingState);
	DOREPLIFETIME(AGameBP, mReplicatableAffectorsRules);
	DOREPLIFETIME(AGameBP, mReplicatedEmergentDifficulty);
}

const TArray<FAffectorInfo>& AGameBP::GetActiveAffectorInfo() const{
	return mActiveAffectorInfo;
}

void AGameBP::OnRep_ObjectiveLocations() {
	OnObjectiveLocationsChanged.Broadcast();
}

void AGameBP::OnRep_ChallengeRewardLocations() {
	OnChallengeRewardLocationsChanged.Broadcast();
}

void AGameBP::OnRep_TrialDefs() {
	if (mCurrentInitState >= EGameBPInit_InitTrials)
	{
		//D11.SC Only fire the prepare trials off if we have received their new replicated values after the init stage has passed the generation stage as it modifies global data used within the generation job
		PrepareTrials();
	}
}

void AGameBP::OnRep_LevelSettings() {
	mHasReceivedReplicatedLevelSettings = true;
	UE_LOG(LogMultiplayer, Log, TEXT("Level settings was replicated with level name: %s"), *mReplicatedLevelSettings_ONLY_FOR_INITIAL_REPLICATION.getLevelDisplayName().ToString());
}

void AGameBP::IncrementRaidDifficulty()
{
	FEmergentDifficulty newEmergentDifficulty = mGame->settings().difficulty.emergentDifficulty().GetNextRaidDifficulty();
	if (HasAuthority()) {
		mReplicatedEmergentDifficulty = newEmergentDifficulty;
		OnRep_ReplicatedEmergentDifficulty();
	}

	if (ADungeonsGameMode* GameMode = Cast<ADungeonsGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->OnDifficultyParametersChanged.Broadcast();
	}

	if (UDungeonsGameInstance* GameInstance = GetDungeonsGameInstance())	{
		FLevelSettings newLevelSettings = GameInstance->GetLevelSettingsLastStarted();
		newLevelSettings.emergentDifficulty = newEmergentDifficulty;
		GameInstance->SetLevelSettingsLastStarted(newLevelSettings);
	}
}

void AGameBP::AddMidGameAffector() {
	TArray<ABaseCharacter*> BaseCharacters = actorquery::getActors<ABaseCharacter>(GetWorld());
	affector::applyRandomAffector(BaseCharacters);

	FEmergentDifficulty newEmergentDifficulty = mGame->settings().difficulty.emergentDifficulty().GetWithChangedAffectorsNum(affector::get(GetWorld()).MidGameAffectorsNum());
	if (HasAuthority()) {
		mReplicatedEmergentDifficulty = newEmergentDifficulty;
		OnRep_ReplicatedEmergentDifficulty();
	}
}

void AGameBP::OnRep_ReplicatedEmergentDifficulty()
{
	OnGameSettingsInitiallized.RemoveDynamic(this, &AGameBP::OnRep_ReplicatedEmergentDifficulty);
	if (mGame != nullptr)
	{
		mGame->SetNewEmergentDifficulty(UDifficultyUtil::GetDifficultyWithEmergentDifficulty(mGame->settings().difficulty, mReplicatedEmergentDifficulty));
		OnDifficultyIncreased.Broadcast(mReplicatedEmergentDifficulty.raidDifficulty);
	}
	else
	{
		OnGameSettingsInitiallized.AddDynamic(this, &AGameBP::OnRep_ReplicatedEmergentDifficulty);
	}
}

void AGameBP::OnAffectorsChanged() {
	FReplicatableAffectorsRules newAffectorRules;
	for (const auto& AffectorInst : affector::get(GetWorld()).GetActive()) {
		newAffectorRules.modifiers.Add({ AffectorInst.Type.Id, AffectorInst.Data });
	}
	mReplicatableAffectorsRules = newAffectorRules;
	OnRep_AffectorsChanged();
	OnAffectorsChangedDelegate.Broadcast();
}

bool MobSpawnQueue::ProcessMobSpawnQueue(UWorld* pWorld, bool bLoadingScreenActive)
{
	if ( AnyPendingSpawns() )
	{
		TSharedPtr<MobSpawnData> MobSpawn;
		MobEndSpawnData EndSpawn;
		if (mMobEndSpawnQueue.Dequeue(EndSpawn))
		{
			//we have previously spawned this one, lets finish it
			game::mobspawn::endSpawnMobActor(EndSpawn.SpawnedMobCharacter, EndSpawn.WorldTransform, EndSpawn.EnchantmentData, EndSpawn.MobSpawnConfig);
			EndSpawn.PostSpawnFunc(EndSpawn.SpawnedMobCharacter);
		}
		else if (mMobSpawnQueue.Dequeue(MobSpawn))
		{
			//spawn this mob and push it to the queue to be finished next tick
			MobSpawn->MobEndData.SpawnedMobCharacter = game::mobspawn::startSpawnMobActor(*pWorld, MobSpawn->MobClass, MobSpawn->MobType, MobSpawn->MobEndData.WorldTransform, MobSpawn->MobEndData.MobSpawnConfig);
						//dont bother if we are in the loading screen
			if (bLoadingScreenActive)
			{
				game::mobspawn::endSpawnMobActor(MobSpawn->MobEndData.SpawnedMobCharacter, MobSpawn->MobEndData.WorldTransform, MobSpawn->MobEndData.EnchantmentData, MobSpawn->MobEndData.MobSpawnConfig);
				MobSpawn->MobEndData.PostSpawnFunc(MobSpawn->MobEndData.SpawnedMobCharacter);
			}
			else
			{
				mMobEndSpawnQueue.Enqueue(MobSpawn->MobEndData);
			}

		}

		return true;
	}

	return false;
}

void FLevelGenerationAsyncTask::DoWork()
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_UEngine_AGameBP_GenerateLevel_);

	const auto levelName = mInitialisingLevelSettings.getLevelName();
	const auto& mission = missions::get(levelName);
	//Init random seed
	const auto levelSeed = mInitialisingLevelSettings.getSeed();

	const auto levelFileName = mInitialisingLevelSettings.getLevelFilename();

	FPlatformMisc::LowLevelOutputDebugStringf(TEXT("\n\n################### AGameBP::GenerateLevel %s Seed (%d) ##################\n"), *levelFileName, levelSeed);
	const auto genData = levelgen::createLevelGenData(mInitialisingLevelSettings);
	FPlatformMisc::LowLevelOutputDebugStringf(TEXT("################### genData (%d) ##################\n"), (int)genData.IsSet());
	if (!genData) {
		return;
	}
	auto result = genData->runner().run(levelSeed, mission.metaScorer());
	FPlatformMisc::LowLevelOutputDebugStringf(TEXT("################### result.success (%d) ##################\n"), (int)result.success);
	if (!result.success) {
		printValidation(result.issues, FString::Printf(TEXT("Can not generate level: %s with seed: %d"), *levelFileName, levelSeed));

		const auto title = FText::FromString(FString("Failed generating level: ") + (result.issues.empty() ? FString() : FString(result.issues.front().title().c_str())));
		const auto summaries = std::accumulate(result.issues.begin(), result.issues.end(), std::string{}, [](const std::string& sum, const Validation& v) {
			return sum + v.summary(false);
		});
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(summaries.c_str()), &title);
		return;
	}

	if (result.levelDef.tiles.empty()) {
		UE_LOG(LogLevelGeneration, Error, TEXT("Generated zero tiles long level: %s with seed: %d"), *levelFileName, levelSeed);
		return;
	}

	if (!result.issues.empty()) {
		const auto prefix = FString::Printf(TEXT("Generation of level: %s and seed: %d completed with issues (e.g. Dungeons not generating)"), *levelFileName, levelSeed);
		printValidation(result.issues, prefix);
	}

	const auto placedCount = result.levelDef.tiles.size();
	const auto placedHash = hashCode(generator::tilesToPlaceResults(result.levelDef.tiles));
	auto frozen = freezeAllInPlace(result.levelDef.tiles);

	// Do not remove!
	if (levelFileName.ToLower() == FString("invalid") && result.finalSeed == 1) {
		if (placedCount != 155 || placedHash != -1955512979) {
			int a = 0;
		}
	}

	mLevelData.mLevelDef = std::make_unique<game::LevelDef>(result.levelDef, result.finalSeed);
	mLevelData.mTileSet = std::move(frozen.tiles);
	mLevelData.mRegion = std::move(createChunkBlockSourceFromTilePlacements(PlacedTiles(frozen.placeResults).placements())); //@placed @lazy

	postprocess::Result processResult;
	{
		FPlatformMisc::LowLevelOutputDebugStringf(TEXT("################### postprocess ##################\n"));
		QUICK_SCOPE_CYCLE_COUNTER(STAT_UEngine_AGameBP_GenerateLevel_levelPostProcessConfig);		
		processResult = run(*mLevelData.mRegion, result.levelDef, mission.levelPostProcessConfig());
	}
	mLevelData.mLevelDef->tileAreas = processResult.tileAreas;
	mLevelData.mFilledDoors = processResult.filledDoors;

	FPlatformMisc::LowLevelOutputDebugStringf(TEXT("\n\n################### AGameBP::GenerateLevel %s Seed (%d) DONE ##################\n"), *levelFileName, levelSeed);
}

void FFadeActorCheckAsyncTask::DoWork()
{
	if (!mWorld.IsValid())
		return;


	const FVector Start = mCurrentCameraPosition;
	mCurrentBlockingActors.Reset();

	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
	ObjectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldDynamic);

	for (auto& PlayerActorData : mCurrentPlayerActors)
	{
		if (!PlayerActorData.mPlayerChar.IsValid()) //no character for some reason?
			continue;

		FVector End = PlayerActorData.mPlayerLocation;

		TArray<FHitResult> OutHits;

		FCollisionQueryParams Params;

		Params.bReturnPhysicalMaterial = true;
		Params.bReturnFaceIndex = false;
		AActor* IgnoreActor = Cast<AActor>(PlayerActorData.mPlayerChar);
		if (IgnoreActor)
		{
			Params.AddIgnoredActor(IgnoreActor);
		}

		//ignore level too
		if (mLevelActor.IsValid())
		{
			Params.AddIgnoredActor(mLevelActor.Get());
		}


		bool const bHit = mWorld->SweepMultiByObjectType(OutHits, Start, End, FQuat::Identity, ObjectParams, FCollisionShape::MakeCapsule(PlayerActorData.mCapsuleRadius, PlayerActorData.mCapsuleHalfHeight), Params);

		if (bHit)
		{

			for (const FHitResult& HitVal : OutHits)
			{

				UPrimitiveComponent* HitComp = HitVal.GetComponent();
				if (HitComp && HitComp->IsA<UStaticMeshComponent>())
				{
					mCurrentBlockingActors.AddUnique(HitVal.Actor.Get());
				}

			}
		}
	}

}

void FFadeActorCheckAsyncTask::GameThreadProcessActors()
{
	if (mCurrentHiddenActors.Num() || mCurrentBlockingActors.Num())
	{

		if (mCurrentBlockingActors.Num())
		{
			//unhide any not still hidden
			for (auto Iter = mCurrentHiddenActors.CreateIterator(); Iter; ++Iter)
			{
				if(!mCurrentBlockingActors.Contains(*Iter))
				{
					auto FadeComponents = (*Iter)->GetComponentsByClass(UMeshFadeBaseComponent::StaticClass());
					for (auto FadeComponent : FadeComponents)
					{
						Cast<UMeshFadeBaseComponent>(FadeComponent)->SetMeshHidden(false);
					}
					Iter.RemoveCurrent();
				}
			}

			//hide any currently not hidden
			for (AActor* HiddenActor : mCurrentBlockingActors)
			{
				if (!mCurrentHiddenActors.Contains(HiddenActor))
				{
					auto FadeComponents = HiddenActor->GetComponentsByClass(UMeshFadeBaseComponent::StaticClass());
					for (auto FadeComponent : FadeComponents)
					{
						Cast<UMeshFadeBaseComponent>(FadeComponent)->SetMeshHidden(true);
					}
				}
			}

			mCurrentHiddenActors = mCurrentBlockingActors;
		}
		else
		{
			for (AActor* HiddenActor : mCurrentHiddenActors)
			{
				auto FadeComponents = HiddenActor->GetComponentsByClass(UMeshFadeBaseComponent::StaticClass());
				for (auto FadeComponent : FadeComponents)
				{
					Cast<UMeshFadeBaseComponent>(FadeComponent)->SetMeshHidden(false);
				}
			}
			mCurrentHiddenActors.Reset();
		}
	}
}

UMeshFadeBaseComponent::UMeshFadeBaseComponent(const FObjectInitializer& ObjectInitializer)
:
	UActorComponent(ObjectInitializer)
{

}
