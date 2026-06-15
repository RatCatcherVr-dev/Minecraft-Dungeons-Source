#include "Dungeons.h"
#include "DungeonsGameMode.h"
#include "DungeonsGameInstance.h"
#include "DungeonsGameSession.h"
#include "DungeonsGameState.h"
#include "game/difficulty/Difficulty.h"
#include "game/Game.h"
#include "game/GameBP.h"
#include "game/GameProgress.h"
#include "game/actor/character/loot/LootActor.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/item/BaseProjectile.h"
#include "game/abilities/effects/DifficultyGameplayEffect.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/effects/AffectorGameplayEffect.h"
#include "game/actor/item/Arrow.h"
#include "game/actor/MapStatActor.h"
#include "game/affector/Affectors.h"
#include "game/component/SecretComponent.h"
#include "game/component/AwardsGeneratorComponent.h"
#include "game/component/HealthComponent.h"
#include "game/component/PlayerExperienceComponent.h"
#include "game/component/ReconnectComponent.h"
#include "game/team/TeamQuery.h"
#include "game/util/ActorQuery.h"
#include "game/util/DungeonsTravelUtil.h"
#include "game/util/EnvironmentUtils.h"
#include "game/util/Tags.h"
#include "game/level/sound/AudioMusicManager.h"
#include "lovika/LovikaLevelActor.h"
#include "online/reconnect/ReconnectUtil.h"
#include "online/sessions/OnlineUtil.h"
#include "online/sessions/SessionSettings.h"
#include "util/CharacterQuery.h"
#include "util/telemetry/Analytics.h"
#include <AbilitySystemComponent.h>
#include "game/abilities/effects/DifficultyGameplayEffect.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/actor/item/Arrow.h"
#include "util/StringUtil.h"
#include "Engine/LocalPlayer.h"
#include "Net/OnlineEngineInterface.h"
#include "game/actor/NotificationBroadcastActor.h"
#include "LogMacros.h"
#include "world/entity/MobTags.h"
#include "TextToSpeechBPLibrary.h"
#include "game/item/instance/AItemInstance.h"
#include "game/actor/item/StorableItem.h"

static int _piePlayerCounter = 0;


DECLARE_STATS_GROUP(TEXT("HealthComponent"), STATGROUP_GameMode, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("OnGameModeDeath"), STAT_GameModeDeath, STATGROUP_GameMode);

ADungeonsGameMode::ADungeonsGameMode() {
	PlayerStateClass = ABasePlayerState::StaticClass();
	GameStateClass = ADungeonsGameState::StaticClass();

	PrimaryActorTick.bCanEverTick = true;
	bAllowTickBeforeBeginPlay = false;
	bUseSeamlessTravel = !environment::startedFromEditor();

	_piePlayerCounter = 0;
}

void ADungeonsGameMode::StartPlay() {
	UE_LOG(LogTemp, Log, TEXT("StartPlay"));

	Super::StartPlay();

	const auto worldType = GetWorld()->WorldType;
	if (worldType == EWorldType::Game) {
		UE_LOG(LogTemp, Warning, TEXT("Using seamless travel"));
		bUseSeamlessTravel = true;
	}
	GameInstance = Cast<UDungeonsGameInstance>(GetGameInstance());

	SetupActorStatsTracking();

	if (!GameBP.IsValid()) {
		GameBP = actorquery::getFirstActor<AGameBP>(GetWorld());
	}	
	//D11.PS we need to force the session data to update once when getting into a session
	ForceOnlineUpdate = true;
}

void ADungeonsGameMode::SetupActorStatsTracking() {
	TrackExistingActorStats();

	const auto actorSpawnedDelegate = FOnActorSpawned::FDelegate::CreateUObject(this, &ADungeonsGameMode::TrackNonPooledActorSpawnedStats);
	GetWorld()->AddOnActorSpawnedHandler(actorSpawnedDelegate);

	FWorldDelegates::LevelAddedToWorld.AddUObject(this, &ADungeonsGameMode::OnLevelAddedToWorld);
}

void ADungeonsGameMode::OnLevelAddedToWorld(ULevel* InLevel, UWorld*) {
	// There might exist some (enabled) pooled items in the level as well
	// but we have too little knowledge about them for tracking them here
	// so we just skip them.
	TrackNonPooledActorSpawnedStats(InLevel->Actors);
}

void ADungeonsGameMode::EndPlay(EEndPlayReason::Type EndPlayReason) {
	UE_LOG(LogTemp, Log, TEXT("DungeonsGameMode::EndPlay"));
	Super::EndPlay(EndPlayReason);
}

void ADungeonsGameMode::StartToLeaveMap() {
	Super::StartToLeaveMap();

	if (HasAuthority() && GameInstance.IsValid()) {
		online::updateOnlineSession(GetWorld());
	}
}

void ADungeonsGameMode::InitGameState() {
	Super::InitGameState();
	UE_LOG(LogTemp, Log, TEXT("InitGameState"));
	gameState = GetGameState<ADungeonsGameState>();
	gameState->OnPlayerRemoved.AddUObject(this, &ADungeonsGameMode::RemovePlayer);
}

FString ADungeonsGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) {	
	UE_LOG(LogTemp, Log, TEXT("InitNewPlayer"));
	reconnect::connect(UGameplayStatics::ParseOption(Options, "guid"), NewPlayerController);
	online::updateOnlineSession(GetWorld(), ForceOnlineUpdate);
	ForceOnlineUpdate = false;
	return Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
}

void ADungeonsGameMode::InitDifficulty(const FLevelSettings& levelSettings) {
	OnInitDifficulty.Broadcast(levelSettings);
}

void ADungeonsGameMode::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	_updatePendingPlayers();
	const auto now = FApp::GetCurrentTime();
	if (now - LastSessionUpdateSec > 10) {
		online::updateOnlineSession(GetWorld());
		LastSessionUpdateSec =  now;
	}
}

void ADungeonsGameMode::_updatePendingPlayers() {
	if (mPendingPlayers.Num() <= 0) {
		return;
	}

	auto gameBp = GetOrFindGame();
	if (gameBp == nullptr) {
		//No gameBP to handle new players
		return;
	}

	if (!gameBp->IsInitialisationComplete()) {
		//Not completed init yet
		return;
	}

	auto game = gameBp->GetGame();
	if (game == nullptr) {
		//No game to handle new players - gamebp is not inited.
		return;
	}

	if (!gameBp->IsReadyForPlayers()) {
		// The game is not fully loaded
		return;
	}

	// #D11.CM - If we're locally controlled, and there are players already in the game, then we must be a drop in player.
	int initialPlayerCount = game->getPlayers().Num();
	for (auto& player : mPendingPlayers) {
		if (!player.IsValid()) {
			continue;
		}

		auto playerCharacter = player.Get();

		if (!playerCharacter->Server_IsReadyToPlay()) {
			continue;
		}

		game->addPlayer(*playerCharacter, initialPlayerCount > 0 && playerCharacter->IsLocallyControlled(), gameBp->IsBeginningOfGame_ServerOnly());
		playerCharacter->SetWorldState(ECharacterWorldState::InWorld);
		playerCharacter->OnAddedToLevel();
		playerCharacter->BindEquipmentSlots();
		ApplyPlayerDifficultyEffects(*playerCharacter);

		OnPlayerCharacterAdded.Broadcast(playerCharacter);				
	}

	mPendingPlayers.RemoveAll([](const TWeakObjectPtr<APlayerCharacter>& player) {
		return !player.IsValid() || player.Get()->GetWorldState() == ECharacterWorldState::InWorld;
	});
}

APawn* ADungeonsGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) {
	const auto notCastPlayer = Super::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);
	check(notCastPlayer && "Could not cast spawn pawn for new player"); //If this fails, it could be because of spawn transform issues (startspots colliding?)
	const auto player = Cast<APlayerCharacter>(notCastPlayer);
	check(player && "Could not cast new pawn to a player character");
	check(!gameState->IsPendingRemoval(player) && "Can not add player already pending removal");

	if(GetOrFindGame()){
		//If a player is spawned in a development level (PIE without a BP_Game present in level)
		//the editor will deal with teleportation and positioning of player.
		//If a player is spawned in a game level with a BP_Game present, the ADungeonsGameMode will handle
		//the player positioning and spawning. When that happens we will make the player appear again.
		player->SetWorldState(ECharacterWorldState::Disappeared);
		mPendingPlayers.Add(player);
	}
	return player;
}

void ADungeonsGameMode::ResetLevel() {
	Super::ResetLevel();
}

void ADungeonsGameMode::RemovePlayer() const {
	OnPlayerCharacterRemoved.Broadcast();
}

void ADungeonsGameMode::OnJoinedPlayerNameChanged(ABasePlayerState* playerState) {
	TArray<ELevelNames> PreventedLevelNames = { ELevelNames::squidcoast };
	auto gameInstance = Cast<UDungeonsGameInstance>(GetGameInstance());
	if (!PreventedLevelNames.Contains(gameInstance->Configuration.GetLevelName())) {
		if (auto notificationBroadcast = actorquery::getFirstActor<ANotificationBroadcastActor>(GetWorld())) {
			notificationBroadcast->MulticastPlayerJoinedNotification(playerState);
		}
	}
	playerState->OnPlayerNameChanged.RemoveDynamic(this, &ADungeonsGameMode::OnJoinedPlayerNameChanged);
}

void ADungeonsGameMode::GamePlayerCountChanged()
{
	OnDifficultyParametersChanged.Broadcast();
}

void ADungeonsGameMode::ApplyDifficultyEffects(AMobCharacter& mob) {
	auto abilitySystem = mob.GetAbilitySystemComponent();
	auto isSpecialMob = mob.IsEnchanted() || hasMobTag(mob.EntityType, MobTags::HashTag_Special);
	if (isSpecialMob) {
		abilitySystem->ApplyGameplayEffectToSelf(effects::DefaultObject<USpecialDifficultyGameplayEffect>(), 1.0f, abilitySystem->MakeEffectContext());
	}
	else {
		abilitySystem->ApplyGameplayEffectToSelf(effects::DefaultObject<UNormalDifficultyGameplayEffect>(), 1.0f, abilitySystem->MakeEffectContext());
	}
	affector::applyAffectorsForCharacter(mob);
}

void ADungeonsGameMode::ApplyDifficultyEffects(AMobCharacter* mob)
{
	ApplyDifficultyEffects(*mob);
}

void ADungeonsGameMode::ApplyPlayerDifficultyEffects(ABaseCharacter& character) {
	auto abilitySystem = character.GetAbilitySystemComponent();
	abilitySystem->ApplyGameplayEffectToSelf(effects::DefaultObject<UPlayerDifficultyGameplayEffect>(), 1.0f, abilitySystem->MakeEffectContext());
	if (Cast<AMobCharacter>(&character)) {
		affector::applyAffectorsForCharacter(character);
	}
}

void ADungeonsGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) {
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	FString levelName = UGameplayStatics::ParseOption(Options, TEXT("level"));
	auto gameInstance = Cast<UDungeonsGameInstance>(GetGameInstance());

	if (levelName != GetEnumValueToStringStripped(gameInstance->Configuration.GetLevelName())) {
		if (auto levelSettings = gameInstance->Configuration.GetLevelSettings()){
			UE_LOG(LogGameMode, Warning, TEXT("Server is on a different level."));
			ErrorMessage = "The host has changed level, please reconnect.";
			return;
		}
	}

	TArray<ABasePlayerController*> Actors = actorquery::getActors<ABasePlayerController>(GetWorld());

	ABasePlayerController** ActorPtr = Actors.FindByPredicate([&UniqueId](const ABasePlayerController* Player)
	{
		return nullptr != Player->PlayerState->UniqueId && UniqueId == *Player->PlayerState->UniqueId;
	});

	ABasePlayerController* PlayerController = ActorPtr ? *ActorPtr : nullptr;

	if(PlayerController)
	{
		if (PlayerController->GetPawn() != nullptr)
		{
			PlayerController->GetPawn()->Destroy();
		}
		PlayerController->Destroy();
	}

}

void ADungeonsGameMode::PostLogin(APlayerController* newPlayer) {

	//If a local player is trying to drop in ingame, we need to stop the login, and do it after character creation. We don't want to stop players transitions, so lets check if they were present before.
	if (Cast<ABasePlayerController>(newPlayer) &&
		newPlayer->IsLocalController() && 
		newPlayer->GetLocalPlayer() != GetWorld()->GetGameInstance()->GetLocalPlayerByIndex(0) &&
		Cast<UDungeonsGameInstance>(GetGameInstance())->localPlayersThatCanLogin.Contains(newPlayer->GetLocalPlayer()) == false)
	{
		return;
	}

	Super::PostLogin(newPlayer);
	UE_LOG(LogTemp, Log, TEXT("PostLogin"));
	const auto player = Cast<APlayerCharacter>(newPlayer->GetPawn());
	if (player == nullptr) {
		return;
	}

	UDungeonsGameInstance* gi = Cast<UDungeonsGameInstance>(GetGameInstance());

	int number = gi->FreeSlots.Num() ? gi->FreeSlots.Pop() : gi->PlayerNumberCounter++;
	auto* playerState = Cast<ABasePlayerState>(newPlayer->PlayerState);
	playerState->SetPlayerNumber(number);

	UpdatePlayerRespawnTimes(GetNumPlayers() > 1 ? RespawnTimeSecondsMultiplayer : RespawnTimeSecondsSingleplayer);

	if (auto playerController = Cast<ABasePlayerController>(newPlayer)) {
		OnPlayerControllerLogin.Broadcast(playerController);

		auto newPlayerId = playerController->GetUniqueNetId();

		if (gi->IsLocalCoop())
		{
			playerController->SetCoopGamepadLightColourIndex(number);

			if(gi->GetNumLocalPlayers() == 2)
			{
				APlayerControllerBase* initialLocalPlayerController = Cast<APlayerControllerBase>(gi->GetFirstLocalPlayerController(GetWorld()));
				initialLocalPlayerController->SetCoopGamepadLightColourIndex(0);
			}
		}

		gameState->AddPlayerController(playerController);

		if (auto ps = Cast<ABasePlayerState>(playerController->PlayerState)) {
			ps->OnPlayerNameChanged.AddDynamic(this, &ADungeonsGameMode::OnJoinedPlayerNameChanged);
		}
	}

	//D11.KS - Refresh all display names. (D11.JPhoenix - Important that this is the last thing we do in this function.
	for (auto lp : GetGameInstance()->GetLocalPlayers())
	{
		if (auto pc = lp->GetPlayerController(GetWorld()))
		{
			if (auto lpPlayerState = Cast<ABasePlayerState>(pc->PlayerState))
			{
				lpPlayerState->RefreshDisplayName();
			}
		}
	}
}

void ADungeonsGameMode::UpdatePlayerRespawnTimes(const float respawnTimeSeconds) const {
	for (auto&& player : TActorRange<APlayerCharacter>(GetWorld())) {
		auto* playerState = Cast<ABasePlayerState>(player->GetDungeonsBasePlayerState());
		if (playerState) {
			playerState->SetRespawnSeconds(respawnTimeSeconds);
		}
	}
}

void ADungeonsGameMode::OnActorHeal(AActor* actor, const float amount) {
	if (const auto player = Cast<APlayerCharacter>(actor)) {
		GetGameStatTracker().TrackScalar(EGameTrackingTypes::PlayerHealingDone, amount);
		getEnsuredPlayerStatTracker(player).TrackScalar(DungeonsPlayerStatTracker::ScalarTrackingTypes::HealingTaken, amount);
	}
}

void ADungeonsGameMode::OnActorDamage(AActor* actor, const float damage, AActor* source, AActor* sourceWeapon) {
	if (const auto player = Cast<APlayerCharacter>(actor)) {
		GetGameStatTracker().TrackScalar(EGameTrackingTypes::PlayerDamageTaken, damage);
		getEnsuredPlayerStatTracker(player).TrackScalar(DungeonsPlayerStatTracker::ScalarTrackingTypes::DamageTaken, damage);
	}
	else if (actor->IsA<AMobCharacter>()) {
		if (const auto sourcePlayer = Cast<APlayerCharacter>(source)) {
			GetGameStatTracker().TrackScalar(EGameTrackingTypes::DamageDealt, damage);
			getEnsuredPlayerStatTracker(sourcePlayer).TrackScalar(DungeonsPlayerStatTracker::ScalarTrackingTypes::DamageDealt, damage);
		}
	}
}

void ADungeonsGameMode::OnActorUsedItem(const AActor* actor, const AItemInstance* item) {
	if (const auto player = Cast<const APlayerCharacter>(actor)) {
		if (HasAuthority()) {
			getEnsuredPlayerStatTracker(player).TrackItemUsed(item->GetItemId());
		}
	}

	ActorUsedItem.Broadcast(actor, item);
}

void ADungeonsGameMode::OnActorSpawnedProjectile(const AActor* actor, const ABaseProjectile* projectile) {
	if (const auto player = Cast<const APlayerCharacter>(actor)) {
		getEnsuredPlayerStatTracker(player).TrackScalar(DungeonsPlayerStatTracker::ScalarTrackingTypes::ProjectileFired, 1);
		GetGameStatTracker().TrackStatEvent(EGameTrackingTypes::PlayerProjectileFired);
	}
}

void ADungeonsGameMode::OnActorSpawnedProjectileHit(const AActor* owner, const ABaseProjectile* projectile) {
	if (const auto player = Cast<const APlayerCharacter>(owner)) {
		getEnsuredPlayerStatTracker(player).TrackScalar(DungeonsPlayerStatTracker::ScalarTrackingTypes::ProjectilesHit, 1);
		GetGameStatTracker().TrackStatEvent(EGameTrackingTypes::PlayerProjectileHit);
	}
}

void ADungeonsGameMode::TrackExistingActorStats() {
	TrackNonPooledActorSpawnedStats(InstanceTracker<AMobCharacter>::GetList(GetWorld()));
	TrackNonPooledActorSpawnedStats(InstanceTracker<AStorableItem>::GetList(GetWorld()));
	TrackNonPooledActorSpawnedStats(actorquery::getActors<ALootActor>(GetWorld()));
}

namespace dungeonsgamemode {

bool shouldMobBeIncludedInStats(const AMobCharacter& mob) {
	return !mob.ActorHasTag(tags::sequencerActor) &&
	       teamquery::is::hostile(ETeamName::Heroes, mob.GetCurrentTeam()) &&
	       characterquery::is::targetable(&mob);
}

}

void ADungeonsGameMode::TrackNonPooledActorSpawnedStats(AActor* actor) {
	if (auto* mob = Cast<AMobCharacter>(actor)) {
		mob->ShouldBeIncludedInStats_ServerOnly = dungeonsgamemode::shouldMobBeIncludedInStats(*mob);

		if (mob->ShouldBeIncludedInStats_ServerOnly) {
			GetGameStatTracker().TrackMobSpawned(mob->EntityType);
		}
	}
	else if (auto item = Cast<AStorableItem>(actor)) {
		if (!item->GetItemType().hasTag(ItemTag::Currency) && !item->GetItemType().isInstant()) {
			GetGameStatTracker().TrackScalar(EGameTrackingTypes::GearSpawned, 1);
		}
	}
	else if (auto lootChest = Cast<ALootActor>(actor)) {
		if (!actor->FindComponentByClass<USecretComponent>()) {
			GetGameStatTracker().TrackScalar(EGameTrackingTypes::ChestsSpawned, 1);
			auto& mapStatActor = GetEnsuredMapStatTracker();
			if (mapStatActor.IsValidLowLevel()) {
				mapStatActor.TrackScalar(EMapTrackingTypes::ChestsSpawned, 1);
			}
			// do not Multicast this message?
			lootChest->OnOpenLoot.AddUObject(this, &ADungeonsGameMode::OnOpenLootChest);
		}
	}
}

void ADungeonsGameMode::TrackEmeraldSpawned() {
	GetGameStatTracker().TrackStatEvent(EGameTrackingTypes::EmeraldsSpawned);
}

void ADungeonsGameMode::OnSecretFound(AActor* finder) {
	GetEnsuredMapStatTracker().TrackScalar(EMapTrackingTypes::SecretsFound, 1);
}

void ADungeonsGameMode::OnOpenLootChest(const FVector&, AActor*, AActor*) {
	UE_LOG(LogDungeons, Warning, TEXT("open loot chest"));
	GetGameStatTracker().TrackScalar(EGameTrackingTypes::ChestsOpened, 1);
	GetEnsuredMapStatTracker().TrackScalar(EMapTrackingTypes::ChestsOpened, 1);
}

void ADungeonsGameMode::OnActorDeath(AActor* Actor, AActor* ByWhom, AActor* ByWhat) {
	SCOPE_CYCLE_COUNTER(STAT_GameModeDeath)
	if (!HasAuthority()) {
		return;
	}

	if (const auto player = Cast<APlayerCharacter>(Actor)) {
		OnPlayerDeath(GetGame(), player, ByWhom, ByWhat);
	} else if (const auto mob = Cast<AMobCharacter>(Actor)) {
		OnMobDeath(GetGame(), mob, ByWhom, ByWhat);
	}

	ActorDeath.Broadcast(Actor, ByWhom, ByWhat);
}

void ADungeonsGameMode::OnSecretSpawned(USecretComponent& secret) {
	auto& mapStatActor = GetEnsuredMapStatTracker();
	if (mapStatActor.IsValidLowLevel()) {
		mapStatActor.TrackScalar(EMapTrackingTypes::SecretsSpawned, 1);
	}
	secret.OnSecretFound.AddUObject(this, &ADungeonsGameMode::OnSecretFound);
}

void ADungeonsGameMode::OnPlayerDeath(const game::Game* game, APlayerCharacter* player, AActor* byWhom, AActor* byWhat) {
	getEnsuredPlayerStatTracker(player).TrackScalar(DungeonsPlayerStatTracker::ScalarTrackingTypes::Deaths, 1);
	GetGameStatTracker().TrackScalar(EGameTrackingTypes::PlayerDeaths, 1);

	if (affector::get(GetWorld()).IsInstantGameOver()) {
		for (auto&& p : actorquery::getActors<APlayerCharacter>(GetWorld())) {
			if (p != player) {
				p->Kill();
			}
		}
	}
	analytics::Analytics::GetInstance().FirePlayerKilled(*player, byWhom, byWhat);
	
}

void ADungeonsGameMode::OnMobDeath(const game::Game* game, const AMobCharacter* mob, AActor* byWhom, AActor* byWhat) {
	const auto byPlayer = [byWhom] {
		auto killingPlayer = byWhom;

		if (const auto maybeMob = Cast<AMobCharacter>(byWhom)) {
			killingPlayer = maybeMob->GetMaster();
		}

		return Cast<APlayerCharacter>(killingPlayer);
	}();

	if (byPlayer != nullptr && mob != byWhom) {
		if (mob->ShouldBeIncludedInStats_ServerOnly) {
			getEnsuredPlayerStatTracker(byPlayer).TrackMobKilled(mob->EntityType);
			GetGameStatTracker().TrackMobKilled(mob->EntityType);
		}

		OnPlayerKilledMob.Broadcast(mob, byWhom, byWhat);
		GrantXPOnMobKill(game, mob);

		analytics::Analytics::GetInstance().FireNPCKilled(*mob, byWhom, byWhat);
		
	}
}

void ADungeonsGameMode::GrantXPOnMobKill(const game::Game* game, const AMobCharacter* mob) const {
	const auto mobKillXp = CalculateMobKillXPValue(game, mob);
	if (mobKillXp > 0) {
		for (auto controllers = GetWorld()->GetPlayerControllerIterator(); controllers; ++controllers) {
			if (const auto playerXpComponent = GetXPComponentFor(*controllers)) {
				playerXpComponent->AddXP_OnlyFromServer(mob->GetActorLocation(), mobKillXp);
			}
		}
	}
}

int ADungeonsGameMode::CalculateMobKillXPValue(const game::Game* game, const AMobCharacter* mob) const {
	const auto multiplier = game != nullptr ? game->settings().difficultyStats.GetXpMultiplier() : 1;
	const UHealthComponent* mobHealthComponent = mob->FindComponentByClass<UHealthComponent>();
	const auto xp = 2.0f + (mobHealthComponent->GetMaximumHealthBase() / 10.0f) * multiplier * FMath::RandRange(0.9f, 1.3f);
	return mob->NoXP ? 0 : FMath::RoundToInt(xp);
}

UPlayerExperienceComponent* ADungeonsGameMode::GetXPComponentFor(const TWeakObjectPtr<APlayerController>& controller) {
	if (controller.IsValid()) {
		if (const auto pawn = controller->GetPawn()) {
			return pawn->FindComponentByClass<UPlayerExperienceComponent>();
		}
	}
	return nullptr;
}

void ADungeonsGameMode::Logout(AController* Exiting) {

	ABasePlayerController* playerController = Cast<ABasePlayerController>(Exiting);

	//D11.KS - Don't log out players that are mid way through dropping in. They never logged in.
	if (playerController) {
		if (auto playerState = Cast<ABasePlayerState>(playerController->PlayerState)) {
			if(playerState->bLocallyDroppingIn) {
				return;
			}
		}
	}

	Super::Logout(Exiting);

	UpdatePlayerRespawnTimes((GetNumPlayers() - 1) > 1 ? RespawnTimeSecondsMultiplayer : RespawnTimeSecondsSingleplayer);

	if (playerController) {
		UDungeonsGameInstance* gi = Cast<UDungeonsGameInstance>(GetGameInstance());

		playerController->OnServerLogout.Broadcast();
		OnPlayerControllerLogout.Broadcast(playerController);

		//D11.KS - newPlayerId was returning null.
		auto netId = playerController->GetUniqueNetId();
		if (netId.IsValid() && netId->IsValid()) 
		{
			const auto nickName = online::getIdentityInterface()->GetPlayerNickname(*netId);
			analytics::Analytics::GetInstance().FireEventPlayerLogout(netId->ToString(), nickName);
		}

		if (auto playerState = Cast<ABasePlayerState>(playerController->PlayerState)) {
			gi->FreeSlots.Add(playerState->GetPlayerNumber());
			gi->FreeSlots.Sort([](const int& LHS, const int& RHS) { return LHS > RHS; });

		}

		gameState->RemovePlayerController(playerController);

		if (auto notificationBroadcast = actorquery::getFirstActor<ANotificationBroadcastActor>(GetWorld())) {
			if (auto playerState = Cast<ABasePlayerState>(playerController->PlayerState)) {
				notificationBroadcast->MulticastPlayerLeftNotification(playerState);
			}
		}
	}
}

void ADungeonsGameMode::OnActorAttackFinished(const AActor* actor, unsigned targetsHit, float damageDone) {
	if (const auto player = Cast<const APlayerCharacter>(actor)) {
		getEnsuredPlayerStatTracker(player).TrackScalar(DungeonsPlayerStatTracker::ScalarTrackingTypes::MostDamageDeltInSingleBlow, damageDone);
		getEnsuredPlayerStatTracker(player).TrackScalar(DungeonsPlayerStatTracker::ScalarTrackingTypes::MostEnemiesHitInSingleBlow, targetsHit);
	}
}

void ADungeonsGameMode::OnActorSteppedOnNewBlock(const AActor* actor, EMaterialTypeEnum blockMaterial) {
	if (const auto player = Cast<const APlayerCharacter>(actor)) {
		getEnsuredPlayerStatTracker(player).TrackBlockTypeWalkedOn(blockMaterial);
	}
}

bool ADungeonsGameMode::KickPlayer(APlayerController* playerController, const FText& KickReason) {
	auto* gameInstance = (UDungeonsGameInstance*) GetGameInstance();
	const UReconnectComponent* reconnectComponent = playerController != nullptr ? playerController->FindComponentByClass<UReconnectComponent>() : nullptr;

	if (GameSession && playerController && gameInstance && reconnectComponent) {
		const FString playerName = playerController->PlayerState != nullptr ? playerController->PlayerState->GetPlayerName() : "Unknown";
		const FString playerGuid = reconnectComponent->GetGuid();

		if (GameSession->KickPlayer(playerController, KickReason)) {
			UE_LOG(LogTemp, Log, TEXT("Kicked player '%s' (%s), reason='%s'"), *playerName, *playerGuid, *KickReason.ToString());
			UTextToSpeechBPLibrary::PlayTextToSpeech(FText::FromString(playerName + " Was Kicked From the game "), true);
			return true;
		}

		UE_LOG(LogTemp, Warning, TEXT("Failed to kick player '%s' (%s), reason='%s'."), *playerName, *playerGuid, *(KickReason.ToString()));
		return false;
	}

	UE_LOG(LogTemp, Error, TEXT("Failed to kick a player, playerController=%p, GameSession=%p, GameInstance=%p, ReconnectComponent=%p."), (void*) playerController, (void*) GameSession, (void*) gameInstance, (void*) reconnectComponent);
	return false;
}

void ADungeonsGameMode::LevelGenerated(const FLevelSettings& LevelSettings) {
	if (HasAuthority()) {
		if (UDungeonsGameInstance* gameInstance = Cast<UDungeonsGameInstance>(GetGameInstance())) {
			online::updateOnlineSession(GetWorld());
			gameInstance->GetReconnectStates().ClearOnlyRestoreStates();
		}
	}
}

TSubclassOf<AGameSession> ADungeonsGameMode::GetGameSessionClass() const {
	return ADungeonsGameSession::StaticClass();
}

const TMap<int, DungeonsPlayerStatTracker>& ADungeonsGameMode::GetPlayerTrackers() const {
	return PlayerStatTrackers;
}

AGameBP* ADungeonsGameMode::GetOrFindGame() {
	if (!GameBP.IsValid()) {
		GameBP = actorquery::getFirstActor<AGameBP>(GetWorld());
	}
	return GameBP.IsValid() ? GameBP.Get() : nullptr;
}

AMapStatActor& ADungeonsGameMode::GetEnsuredMapStatTracker()
{
	if (!MapStatActor.IsValid()) {
		auto mapStatActor = actorquery::getFirstActor<AMapStatActor>(GetWorld());
		MapStatActor = mapStatActor ? mapStatActor : Cast<AMapStatActor>(GetWorld()->SpawnActor(AMapStatActor::StaticClass()));
	}
	return *MapStatActor.Get();
}

DungeonsPlayerStatTracker& ADungeonsGameMode::getEnsuredPlayerStatTracker(const APlayerCharacter* player) {
	if (!player || !player->GetDungeonsBasePlayerState()) {
		return DummyPlayerStatTracker;
	}
	const auto key = player->GetDungeonsBasePlayerState()->GetPlayerNumber();
	return PlayerStatTrackers.FindOrAdd(key);
}

FMissionFinishedSummary ADungeonsGameMode::GenerateMissionFinishedSummary() {
	FMissionFinishedSummary summary{};

	auto gameInstance = GetGameInstance<UDungeonsGameInstance>();
	ensure(gameInstance && "Unexpectedly gameInstance is not what we need. This will most likely cause strange problems with reward generation espectially");
	if (gameInstance) {
		summary.levelSettings = gameInstance->GetLevelSettingsLastStarted();
		summary.trialIdOrBlank = gameInstance->GetTrialIdLastStarted().Get("");
	}

	if (AGameBP* gameBP = GetOrFindGame()) {
		summary.levelModifiers = gameBP->GetGameModifiers();
	}

	const auto awardComponent = FindComponentByClass<UAwardsGeneratorComponent>();
	ensure(awardComponent && "This will not crash - but now we have no awards in the post game screen.");
	if (awardComponent) {
		summary.awards = awardComponent->GenerateAwards(summary.levelSettings.getLevelName(), summary.levelSettings.getDifficulty());
		summary.gameStats = awardComponent->ProcessGameStats();
	}

	return summary;
}

game::Game* ADungeonsGameMode::GetGame() const {
	return GameBP.IsValid() ? GameBP->GetGame() : nullptr;
}

void ADungeonsGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) {
	Super::InitGame(MapName, Options, ErrorMessage);

	FString controllerPath = "/Game/Actors/Characters/Player/BP_PlayerController";
	PlayerControllerClass = ConstructorHelpersInternal::FindOrLoadClass(controllerPath, APlayerController::StaticClass());

	FString pawnPath = "/Game/Actors/Characters/Player/Alex/BP_AlexCharacter";
	DefaultPawnClass = ConstructorHelpersInternal::FindOrLoadClass(pawnPath, APawn::StaticClass());
}

