#include "Dungeons.h"
#include "Game.h"
#include "GameProgress.h"
#include "DamageHelpers.h"
#include "DungeonsGameInstance.h"
#include "DungeonsGameMode.h"
#include "DungeonsGameState.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/affector/Affectors.h"
#include "game/ai/BehaviorSystem.h"
#include "game/component/EquipmentComponent.h"
#include "game/component/HealthComponent.h"
#include "game/component/RagdollOnDeathComponent.h"
#include "game/component/ReconnectComponent.h"
#include "game/component/TeleportComponent.h"
#include "game/cinematic/CinematicSequence.h"
#include "game/event/TileEvents.h"
#include "game/event/TileEventDispatcher.h"
#include "game/level/GameTiles.h"
#include "game/level/LootSpawner.h"
#include "game/level/StaticMeshSpawner.h"
#include "game/level/StaticToInstancedMeshConverter.h"
#include "game/level/SubLevelTileLoader.h"
#include "game/level/TileDecor.h"
#include "game/level/TilePreparer.h"
#include "game/level/doors/DoorUtil.h"
#include "game/level/environment/Environment.h"
#include "game/level/ambience/AmbienceTracker.h"
#include "game/mobspawn/EntityTypeMappers.h"
#include "game/mobspawn/MobSpawnConfigs.h"
#include "game/mobspawn/alpha/AlphaSpawner.h"
#include "game/mobspawn/alpha/HyperSpawner.h"
#include "game/mobspawn/fixed/FixedSpawner.h"
#include "game/mobspawn/event/EventMobSpawner.h"
#include "game/mobspawn/RaidCaptain/RaidCaptainSpawner.h"
#include "game/Navigation/NavBoundsVolume.h"
#include "game/objective/ChallengeSystem.h"
#include "game/objective/ObjectivesSystem.h"
#include "game/objective/ObjectiveFactory.h"
#include "game/objective/RegionFinder.h"
#include "game/trigger/Triggers.h"
#include "lovika/RegionPredicates.h"
#include "online/reconnect/ReconnectUtil.h"
#include "util/StringUtil.h"
#include "util/telemetry/Analytics.h"
#include "Runtime/Engine/Classes/Kismet/KismetGuidLibrary.h"
#include <Engine/LevelStreamingDynamic.h>
#include "NavigationSystem.h"
#include "Core/Public/Misc/CoreDelegates.h"
#include "game/util/EnvironmentUtils.h"
#include "Engine/AssetManager.h"
#include "util/CollectionUtils.h"
#include <limits>
#include "difficulty/DifficultyUtil.h"
#include "item/instance/AItemInstance.h"
#include "affector/AffectorTypes.h"
#include "affector/AffectorData.h"

namespace game {

sublevel::Loader::Config createSublevelLoaderConfig() {
	auto config = sublevel::Loader::DefaultConfig;
	config.immediatelyLoadFirstSublevelsCount = environment::startedFromEditor() ? std::numeric_limits<int>::max() : 0;
	return config;
}

Game::Game(UWorld& world, const LevelDef& levelDef, const FMissionState& missionState, const bool isServer)
	: mWorld(world)
	, mLevelDef(levelDef)
	, mSettings(missionState.missionDifficulty, levelDef.finalSeed, missionState.getLevelName())
	, mMissionState(missionState)
	, mIsServer(isServer)
	, mTiles(levelDef)
	, mSublevelLoader(std::make_unique<sublevel::TileLoader>(world, createSublevelLoaderConfig()))
	, mStaticMeshConverter(std::make_unique<AStaticToInstancedMeshConverter>())
	, mRegionFinder(regionfinders::Default(mTiles.getTiles()))
	, mTileEventDispatcher(std::make_unique<events::TileEventDispatcher>(mTiles))  //D11.PS - added game namespace conflict keyword
	, mTriggers(std::make_unique<trigger::Triggers>(*this))
	, mProgress(std::make_unique<GameProgress>(*this, levelDef.levelDef.data.definedLevelIds))
	, mRoundId(UKismetGuidLibrary::NewGuid())
	, mRoundIdString(stringutil::toStdString(mRoundId.ToString(EGuidFormats::DigitsWithHyphens)))
	, mCachedRoundId(false)
	, mAllowClientConsoleCommands(true)
	, mInitState(0)
{
}

Game::~Game()
{
	if (mPreloadTeleportDoorsHandle) {
		mPreloadTeleportDoorsHandle->CancelHandle();
	}
}

bool Game::LoadAsyncAndPollIntroCinematic() const
{
	ADungeonsGameState* gameState = mWorld.GetGameState<ADungeonsGameState>();

	if (!mLevelDef.levelDef.data.playIntro || IsLobbyLevel()) {
		// Skip intro
		return true;
	}
	else if (!gameState->HasCinematicStartedLoading()) {
		//loading has not started
		auto filename = decor::filenameForActor(FString(levelName().c_str()), FString("intro"));

		if (filename.Contains("/")) {
			int32 index = filename.Find("/", ESearchCase::IgnoreCase, ESearchDir::FromEnd);
			FString Classname = filename.RightChop(index + 1);
			filename = filename + "." + Classname + "_C";
		}

		FSoftObjectPath SoftPath = filename;

		auto tile = mLevelDef.placedTiles[0];
		gameState->StartLoadingCinematic(SoftPath, tile.placement(), tile.tile().posHACK());
	}
	else {
		//check for loading finished
		return !gameState->IsCinematicLoading();
	}

	return false;
}

/** moved out of constructor to be able to find level name from BP to trigger intro audio **/
void Game::StartIntroCinematic() const {
	ADungeonsGameState* gameState = mWorld.GetGameState<ADungeonsGameState>();

	if (!gameState->PlayLoadedCinematic()) {
		gameState->OnCinematicCannotPlay();
	}
}

const std::string& Game::levelName() const {
	return mLevelDef.levelDef.id;
}

UWorld& Game::world() const {
	return mWorld;
}

enum InitState
{
	EGameInit_Start = 0,
	EGameInit_SetupAffectors,
	EGameInit_StartPlaceEnvironmentalEffects,
	EGameInit_EndPlaceEnvironmentalEffects,
	EGameInit_RegisterLevelLoadedCallback,
	EGameInit_StartPreloadTeleportDoors,
	EGameInit_WaitPreloadTeleportDoors,
	EGameInit_PlaceTeleportDoors,
	EGameInit_CreateDecorSubLevels,
	EGameInit_SetupNavMesh,
	EGameInit_Server,
	EGameInit_AmbienceTracker,
	EGameInit_End
};

void Game::update() {
	if (mInitState != EGameInit_End)
	{
		return;
	}

	mPlayers.RemoveAll([](auto&& p) { return p == nullptr; });

	auto players = Util::getRawPointers(mPlayers);
	mTileEventDispatcher->update(players);
	mSublevelLoader->update();

	if (mIsServer) {
		mBehaviorSystem->Update();

		// Only tick certain systems if there are connected players
		if (players.Num() > 0) {
			mEventMobSpawner->update();
			mObjectives->tick();
			mChallenges->tick();
		}
	}
	updatePlayers();
	updateAmbience();
}

bool Game::Init()
{
	switch (mInitState)
	{

	case EGameInit_Start:
	{
		++mInitState;
		break;
	}

	case EGameInit_SetupAffectors:
	{
		mAffectors.Set(missionDef().affectors());
		++mInitState;
		break;
	}

	case EGameInit_StartPlaceEnvironmentalEffects:
	{
		mEnvironment = std::make_unique<game::Environment>(&mWorld, tiles(), mLevelDef.levelDef);
		++mInitState;
		break;
	}

	case EGameInit_EndPlaceEnvironmentalEffects:
	{
		if (mEnvironment->completedLoadEnvironmentalEffects())
		{
			++mInitState;
		}
		break;
	}

	case EGameInit_RegisterLevelLoadedCallback:
	{
		mSublevelLoader->registerLevelLoadedCallback([this](ULevelStreamingDynamic* level) {
		mStaticMeshConverter->Convert(level);
		mEnvironment->add(level->GetLoadedLevel()->Actors);
		});

		++mInitState;
		break;
	}

	case EGameInit_StartPreloadTeleportDoors:
	{

		if (mIsServer)
		{
			mPreloadTeleportDoorsHandle = door::preloadTeleportDoors(mLevelDef.levelDef.graph, [&]() {
				//inc state once loading finished
				++mInitState;
			}
			);
		}
		else
		{
			++mInitState;
		}
			
		++mInitState;
		
		break;
	}

	case EGameInit_WaitPreloadTeleportDoors:
	{
		//Wait for preload callback to iterate us out of this state
		break;
	}

	case EGameInit_PlaceTeleportDoors:
	{
		if (mIsServer) 
			door::placeTeleportDoors(mWorld, mLevelDef.levelDef.graph);
		++mInitState;
		break;
	}

	case EGameInit_CreateDecorSubLevels:
	{
		decor::createDecorSubLevels(mLevelDef, *mSublevelLoader);
		++mInitState;
		break;
	}

	case EGameInit_SetupNavMesh:
	{
		setupNavMesh();
		++mInitState;
		break;
	}

	case EGameInit_Server:
	{
		auto spawnerConfig = mobspawn::configs::DefaultTileData(*this);
		spawnerConfig.TypeMapper(
			mobspawn::CascadedBreakWhenTypeChanged({
				[this](EntityType entityType) { return mAffectors.GetEntityTypeMapper()(entityType); },
				spawnerConfig.config.typeMapper
				})
		);

		if(missionDef().isHyperMission()) {
			mHyperSpawner = std::make_unique<mobspawn::HyperSpawner>(*this, mSettings.difficultyStats, mMissionState, spawnerConfig);
		}

			
		if (mIsServer) {
			mBehaviorSystem = std::make_unique<UBehaviorSystem>(mWorld);
			mObjectives = std::make_unique<objective::ObjectivesSystem>(*this, objective::create(mLevelDef.levelDef.data.objectives));
			mChallenges = std::make_unique<objective::ChallengeSystem>(*this);
			mEventMobSpawner = mobspawn::eventmob::createDefaultEventMobSpawnerForLevel(*this);
			mTilePreparer = std::make_unique<tile::TilePreparer>(*this);

			mTilePreparer->add(mobspawn::FixedSpawner(mWorld, mobspawn::configs::DefaultTileData(*this)));
			// need perhaps to refactor
			mTilePreparer->add(mobspawn::RaidCaptainSpawner(mWorld, mobspawn::configs::DefaultTileData(*this), mMissionState, tiles()));
			mTilePreparer->add(tile::LootSpawner(*this, false));
			mTilePreparer->add(tile::StaticMeshSpawner(mWorld));

			if (mHyperSpawner) {
				mTilePreparer->add(std::ref(*mHyperSpawner));
			} else {
				mTilePreparer->add(mobspawn::AlphaSpawner(mWorld, mSettings.difficultyStats, spawnerConfig));
			}
			setupTriggers();
		}

		++mInitState;
		break;
	}

	case EGameInit_AmbienceTracker:
	{
		mAmbienceTracker = std::make_unique<ambience::AmbienceTracker>(mWorld, mTiles);
		++mInitState;
		break;
	}

	case EGameInit_End:
	{
		return true;
	}

	default:break;
	}
	   
	return false;
}

bool Game::IsLobbyLevel() const {
	return mLevelDef.levelDef.id == "lobby";
}

bool Game::canEnterNight() const {
	return 
		!IsLobbyLevel() &&
		mLevelDef.levelDef.id != "SquidCoast";
}

bool Game::IsInitDone() const
{
	return mInitState == EGameInit_End;
}

bool Game::isCompleted() const {
	return mObjectives && mObjectives->completion().isCompleted();
}

void Game::addPlayer(APlayerCharacter& player, bool isDropIn, bool isBeginningOfGame) {
	DEBUG_ASSERT(!mPlayers.Contains(&player), "Player was already inserted in player collection");
	initPlayerCharacterInLevel(player, isDropIn, isBeginningOfGame);
	mPlayers.Add(&player);
}

void Game::suicide() {
	if (mPlayers.Num() == 0) {
		return;
	}

	if (mPlayers[0].IsValid()) {
		mPlayers[0]->Kill();
	}
}

void Game::forceEndGame() const {
	if(mIsServer) {
		mObjectives->forceCompleteAll();
	}
}

void Game::initPlayerCharacterInLevel(APlayerCharacter& player, bool isDropIn, bool isBeginningOfGame) const {
	bool showIntroToPlayer = false;

	if (reconnect::canRestore(&player)) {
		//Restore player character because they have a state to restore from
		reconnect::restore(&player);
		if (!player.GetTeleportComponent()->TeleportAfterReconnect()) {
			//Fallback if teleport to reconnect tile fails
			player.GetTeleportComponent()->TeleportToStart();
		}
	} else {
		//Create fresh state for player character
		if (auto ec = player.GetEquipmentComponent()) {
			//Remember which equipped gear power the player character started the mission with.
			ec->SetMissionStartingEquippedGearPower(ec->GetTotalEquippedDisplayItemPower());
		}
	
		//Generate the default loadout for new players.
		player.GenerateLoadout(mLevelDef.levelDef.data.characterLoadout);

		if (isDropIn) {
			// #D11.CM - Drop In
			if (auto primaryCharacter = player.GetWorld()->GetFirstPlayerController<ABasePlayerController>()->GetControlledPlayerCharacter()) {

				// Mark our character as off screen.
				player.GetPlayerController()->CurrentCharacterScreenStatus = ELocalPlayerCameraStatus::OffScreen;

				// Pop to the leader
				if (!player.PopToPlayer(primaryCharacter)) {
					// We couldn't teleport to the leader, so treat us as a reconnect.
					player.GetTeleportComponent()->TeleportAfterReconnect();
				}
			}
		}
		else {
			showIntroToPlayer = isBeginningOfGame;
			if (isBeginningOfGame) {
				// Go through normal procedure if we're not a drop in
				player.GetTeleportComponent()->TeleportToStart();
			}
			else {
				player.GetTeleportComponent()->TeleportAfterReconnect();
			}
		}
	}	

	player.PlayerLoadedInLevel(showIntroToPlayer);
	affector::applyAffectorsForCharacter(player);
}

void Game::updateAmbience() const {
	if(mAmbienceTracker && HasLoadedSublevels())
		mAmbienceTracker->updatePlayerAmbience();	
}

bool Game::HasLoadedSublevels() const
{
	return mSublevelLoader->AreAllLevelsLoaded();
}

void Game::updatePlayers() {
	if (mIsServer) {
		updatePlayerCount();
	}	
}

void Game::updatePlayerCount() {
	// We should later use mPlayers.Num(), but since we aren't really removing players, let's use this
	auto const playerCount = InstanceTracker<APlayerCharacter>::GetList(&mWorld).Num();

	if (mSettings.difficultyStats.ChangeNumberOfPlayers(playerCount)) {
		if (auto&& gamemode = Cast<ADungeonsGameMode>(mWorld.GetAuthGameMode())) {
			gamemode->GamePlayerCountChanged();
		}
	}
}

APlayerCharacter* getLocalPlayer(UWorld& world) {
	if (const auto* gameInstance = world.GetGameInstance()) {
		if (const auto* controller = gameInstance->GetFirstLocalPlayerController(&world)) {
			return Cast<APlayerCharacter>(controller->GetCharacter());
		}
	}
	return nullptr;
}

void Game::setupTriggers() const {
	const auto all = filtered(mRegionFinder.get(), regionpredicates::isTrigger());

	// Death regions
	triggers().enterRegion(filtered(all, regionpredicates::isDeathTrigger()), [](const trigger::RegionState& state) {
		const bool isFreezeZone = (state.region.lowerTagString() == "freeze");
		damagehelpers::tryKillByKillZone(state.actor, isFreezeZone);
	});

	// Checkpoints
	tileEvents().tileEnter([&](const events::TileState& state) {
		// @hack: unfortunately the bounds overlap test (that sets the checkpoint) happens
		//        BEFORE this one (which happens at tick), so we need to verify that we aren't
		//        potentially inside the checkpoint (same tile is correct enough). When we
		//        figure out how to do this properly, we can move it to a better place.
		const auto isTileDifferentFromLastCheckpoint = [&] {
			const auto checkpoint = state.player.GetCurrentCheckpoint();
			return !checkpoint || !tiles().isSameTile(state.player, centerFloor(checkpoint.GetValue()));
		};
		if (isTileDifferentFromLastCheckpoint()) {
			state.player.ClearCurrentCheckpoint();
		}
	});
	triggers().enterRegion(filtered(all, regionpredicates::isCheckPoint()), [](const trigger::RegionState& state) {
		if (APlayerCharacter* player = Cast<APlayerCharacter>(&state.actor)) {
			player->SetCurrentCheckpoint(state.region.area());
		}
	});
}

void Game::setupNavMesh() {
	if (auto oldBoundsVolume = actorquery::getFirstActor<ANavMeshBoundsVolume>(&mWorld)) {
		UNavigationSystemV1* navigationSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(&mWorld);
		oldBoundsVolume->SetActorScale3D(FVector::OneVector);
		navigationSystem->OnNavigationBoundsUpdated(oldBoundsVolume);
	}
	for (auto* tile : mTiles.getTiles()) {
		if (auto* navBounds = mWorld.SpawnActor<ANavBoundsVolume>()) {
			navBounds->SetBounds(tile->bounds());
			navBounds->UpdateNavigation();
		}
	}
}

void Game::forceCompleteCurrentObjective() const {
	auto objectives = mObjectives.get();
	if (auto current = objectives->current()) {
		if (!current->completion().isCompleted()) {
			current->forceCompleted();
			objectives->tick();
		}
	}
}

void Game::SetNewEmergentDifficulty(FDifficulty increasedEmergentDifficulty)
{
	mSettings.difficulty = increasedEmergentDifficulty;
	int numberOfPlayers = mSettings.difficultyStats.GetNumberOfPlayers();
	mSettings.difficultyStats = DifficultyStats{ mSettings.difficulty };
	mSettings.difficultyStats.ChangeNumberOfPlayers(numberOfPlayers);
}

const Tiles& Game::tiles() const {
	return mTiles;
}

const game::RegionFinder& Game::regionFinder() const {
	return mRegionFinder;
}

const GameProgress& Game::progress() const {
	return *mProgress;
}

const Settings& Game::settings() const {
	return mSettings;
}

UBehaviorSystem* Game::behaviorSystem() {
	return mBehaviorSystem ? mBehaviorSystem.get() : nullptr;
}

const MissionDef& Game::missionDef() const {
	return missions::get(mSettings.levelName);
}

//D11.PS - added game namespace conflict keyword
events::TileEvents& Game::tileEvents() const { 
	return mTileEventDispatcher->events();
}

const objective::ObjectivesSystem* Game::objectives() const {
	return mObjectives.get();
}

const objective::ChallengeSystem* Game::challenges() const {
	return mChallenges.get();
}

trigger::Triggers& Game::triggers() const {
	return *mTriggers;
}

const affector::Affectors& Game::affectors() const {
	return mAffectors;
}

void Game::SetReplicatedAffectors(const FReplicatableAffectorsRules& replicatedAffectors) {
	mAffectors.Clear();
	mAffectors.Set(replicatedAffectors.ToRuleData());
}

}
