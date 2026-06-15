#include "Dungeons.h"
#include "MissionProgressHandler.h"
#include "DungeonsGameMode.h"
#include "DungeonsGameState.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/item/generator/ItemGenerator.h"
#include "game/objective/Objective.h"
#include "game/objective/objectives/SoundData.h"
#include "game/util/DungeonsTravelUtil.h"
#include "lovika/LovikaLevelActor.h"
#include "util/DungeonsPlayerStatTracker.h"
#include <Net/UnrealNetwork.h>
#include <Sound/SoundBase.h>
#include "game/item/drop/ItemDropGenerator.h"
#include "online/trials/TrialsProvider.h"
#include "online/sessions/OnlineUtil.h"
#include "util/RandomUtil.h"
#include "util/FloatWeighedRandom.h"
#include "util/ConfigFileUtil.h"
#include "util/StringUtil.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/component/CharacterSerializeComponent.h"
#include "game/objective/ObjectiveCameraPanning.h"
#include "game/level/sound/AudioMusicManager.h"
#include "game/mission/MissionDef.h"
#include "Engine/LocalPlayer.h"
#include "hud/DungeonHUD.h"
#include "game/mission/EventLocTexts.h"
#include "game/mission/EndVideoDefinitions.h"
#include "game/mission/state/MissionStateUtil.h"
#include "game/objective/EventTypes.h"
#include "game/progress/ProgressStat.h"
#include "game/actor/DarkSpawner.h"
#include "Kismet/GameplayStatics.h"
#include "game/difficulty/endless/EndlessStruggleTiers.h"
#include "online/seasons/LiveOps.h"
#include "DungeonsUserManagement.h"

AMissionProgressHandler::AMissionProgressHandler() {
	PrimaryActorTick.bCanEverTick = true;
	bAlwaysRelevant = true;	
	SetReplicates(true);

	ObjectiveEventHandler.mOnCameraPanningStarted = [&] { CameraPanningStarted(); };
	ObjectiveEventHandler.mOnCameraPanningComplete = [&] { CameraPanningCompleted(); };
}

void AMissionProgressHandler::Tick(float deltaTime) {
	Super::Tick(deltaTime);

	ObjectiveEventHandler.onTick(deltaTime);
}

void AMissionProgressHandler::SetObjective(const game::objective::Objective& obj) {
	SetDisplayDetails(makeMissionDetails(obj));
}

void AMissionProgressHandler::SetDisplayDetails(const FMissionDetails& missionDetails) {
	if (!NewMissionDetails.Name.IsEmpty()) {
		OnObjectiveComplete.Broadcast(NewMissionDetails.Name, NewMissionDetails.Id.Get(""));
	}

	NewMissionDetails = missionDetails;
	IsVisible = true;
	RepNotifyInternal();
}

void AMissionProgressHandler::SetEmptyObjective() {
	IsVisible = false;
	RepNotifyInternal();
}

void AMissionProgressHandler::SetPartiallyComplete(int count, int objectiveIndex) {
	if (objectiveIndex != NewMissionDetails.ObjectiveIndex) {
		return;
	}
	if (count == mMissionProgressCount) {
		return;
	}

	mMissionProgressCount = count;

	// @note: horrible hack to get rid of the "show an old objective name large in the middle of the screen"
	if (count >= 1 && NewMissionDetails.DisplayMode == EObjectiveDisplayMode::NewMainObjective) {
		NewMissionDetails.DisplayMode = EObjectiveDisplayMode::NewSubobjective;
	}

	if (HasAuthority()) {
		OnRep_MissionUpdated();
	}
}

void AMissionProgressHandler::MissionCancelledTravelToLobby() {
	MulticastInstantMoveToNextLevel(levelsettingsutil::generateLobbySettings(GetWorld()));
}

void AMissionProgressHandler::MissionFinished() {
	if (Role == ROLE_Authority) {

		for (auto* player : InstanceTracker<APlayerCharacter>::GetList(GetWorld())) {
			player->bCanBeDamaged = false;
			
			// D11.BC Stop players moving around after mission end
			if (auto baseController = Cast<ABasePlayerController>(player->GetPlayerController()))
			{
				baseController->AbortPathFollowing();
			}
		}

		//D11.SC Trigger mob destruction during mission exit to reduce stalls getting rid of them all
		if (const auto DarkSpawner = actorquery::getFirstActor<ADarkSpawner>(GetWorld()))
		{
			for (auto* mob : InstanceTracker<AMobCharacter>::GetList(GetWorld()))
			{
				if (mob->mPlayerVisible)
				{
					TWeakObjectPtr < AMobCharacter > WeakMob = mob;
					TWeakObjectPtr < ADarkSpawner > WeakDarkSpawner = DarkSpawner;
					FTimerHandle T1;
					GetWorld()->GetTimerManager().SetTimer(T1, [WeakMob, WeakDarkSpawner]{ if (WeakMob.IsValid() && WeakDarkSpawner.IsValid()) { WeakDarkSpawner->Despawn(WeakMob.Get()); } }, FMath::RandRange(0.01f, 1.0f), false);
				}
				else
				{
					mob->SetLifeSpan(0.001f);
				}
			}
		}
		else
		{
			for (auto* mob : InstanceTracker<AMobCharacter>::GetList(GetWorld())) 
			{
				mob->SetLifeSpan(0.001f);
			}
		}
		
		const auto& missionSummary = GetWorld()->GetAuthGameMode<ADungeonsGameMode>()->GenerateMissionFinishedSummary();
		SetLocalPlayersCompletedMission(missionSummary.levelSettings);

		FLevelSettings TravelSettings;
		
		const auto& currentMissionDef = missions::get(missionSummary.levelSettings.getLevelName());		
	
		if (currentMissionDef.GetProgressToNextMission() == ELevelNames::Invalid)
		{
			//go to lobby
			TravelSettings = levelsettingsutil::generateLobbySettings(GetWorld());
		}
		else
		{
			//create travel info for next mission to go to
			const FMissionDifficulty& CurrentMissionDifficulty = missionSummary.levelSettings.missionState.missionDifficulty;
			FMissionSelection NewMission = { currentMissionDef.GetProgressToNextMission(), CurrentMissionDifficulty.difficulty, CurrentMissionDifficulty.threatLevel, CurrentMissionDifficulty.endlessStruggle };
								
			const auto& nextMissionDef = missions::get(NewMission.levelName);
				
			FMissionState missionState = FMissionState(FMissionDifficulty::from(NewMission), nextMissionDef.provideRandomSeed({}));
			missionState.bCompletedOnce = false;
				
			TravelSettings = levelsettingsutil::generateMissionSettings(GetWorld(), missionState);
		}
		

		MulticastMissionFinished(
			missionSummary,
			TravelSettings
		);
	}
}

void AMissionProgressHandler::GameOver() {
	if (Role != ROLE_Authority) {
		return;
	}
	const auto& levelSettings = GetGameInstance<UDungeonsGameInstance>()->GetLevelSettingsLastStarted();
	MulticastGameOver(
		levelSettings.getLevelName(),
		levelSettings.missionState.guid,
		levelsettingsutil::generateLobbySettings(GetWorld())
	);
}

void AMissionProgressHandler::MoveToNextLevelDelayed(const FLevelSettings& levelSettings, float delay) {
	FTimerDelegate del;
	mNextLevelSettings = levelSettings;
	del.BindUObject(this, &AMissionProgressHandler::MoveToNextLevelSettings);
	GetWorld()->GetTimerManager().SetTimer(mEndMissionHandle, del, delay, false);
}

void AMissionProgressHandler::MoveToNextLevelSettings() const
{
	MoveToNextLevel(mNextLevelSettings);
}

void AMissionProgressHandler::MoveToNextLevel(const FLevelSettings& levelSettings) const {
	if (auto gi = GetGameInstance<UDungeonsGameInstance>()) {
		gi->Configuration.PrepareTravel(levelSettings);
				
		if(levelSettings.isLobby())
		{
			MoveToLobbyLevel(gi, levelSettings);
		}
		else
		{
			MoveToIngameLevel(gi, levelSettings);
		}
	}
}

void AMissionProgressHandler::MoveToLobbyLevel(class UDungeonsGameInstance* pGameInstance, const FLevelSettings& levelSettings) const
{
	if (Role == ROLE_Authority) {
		if (pGameInstance->IsDedicatedServerInstance()) {
			travelutil::ServerTravelToLobby(GetWorld(), levelSettings);
		}
		else {
			pGameInstance->BeginLoadingScreenWithTravel(levelSettings, EMapLoadType::TravelLobbyServer);
		}
	}
	else {
		pGameInstance->BeginLoadingScreenWithTravel(levelSettings, EMapLoadType::TravelLobbyClient);
	}
}

void AMissionProgressHandler::MoveToIngameLevel(class UDungeonsGameInstance* pGameInstance, const FLevelSettings& levelSettings) const
{
	if (Role == ROLE_Authority) {
		if (pGameInstance->IsDedicatedServerInstance()) {
			travelutil::ServerTravelToGameMap(GetWorld(), levelSettings);
		}
		else {
			pGameInstance->BeginLoadingScreenWithTravel(levelSettings, EMapLoadType::TravelIngameServer);
		}
	}
	else {
		pGameInstance->BeginLoadingScreenWithTravel(levelSettings, EMapLoadType::TravelIngameClient);
	}
}


void AMissionProgressHandler::RestartInMenu() const {
	if (auto gi = GetGameInstance<UDungeonsGameInstance>()) {
		if (Role == ROLE_Authority) {
			gi->BeginLoadingScreenWithTravel(FLevelSettings(), EMapLoadType::OpenMenu);
		}
		else {
			gi->BeginLoadingScreenWithTravel(FLevelSettings(), EMapLoadType::TravelLobbyClient);
		}
	}
}

void AMissionProgressHandler::OnRep_MissionUpdated() {
	OnObjectiveUpdated.Broadcast(mMissionProgressCount);
	OnSpecificObjectiveUpdated.Broadcast(NewMissionDetails.Name, NewMissionDetails.Id.Get(""), mMissionProgressCount);
}

void AMissionProgressHandler::MoveEveryoneToLobbyOnce() {
	if (!HasIssuedReturnToLobby) {
		HasIssuedReturnToLobby = true;
		MulticastInstantMoveToNextLevel(levelsettingsutil::generateLobbySettings(GetWorld()));
	}
}

void AMissionProgressHandler::OnEndVideoFinished() {
	ExecuteAfterVideo();
	if (Role == ROLE_Authority) {
		MulticastInstantMoveToNextLevel(mNextLevelSettings);
	}
}

void AMissionProgressHandler::ExecuteBeforeVideo() 
{
	// increase the interval at each Garbage Collector purges unused objects to 3 mins
	// to prevent FPS dips while the video is playing
	GEngine->Exec(nullptr, TEXT("gc.TimeBetweenPurgingPendingKillObjects 183.3"));
	UE_LOG(LogTemp, Log, TEXT("End video started: GC Object purge interval increased."));
}

void AMissionProgressHandler::ExecuteAfterVideo()
{
	// Restore the default Garbage Collector purge interval from UE4/Engine/Config/BaseEngine.ini
	FString defaultGCPurgeValue;
	bool valueFound = GConfig->GetString(TEXT("/Script/Engine.GarbageCollectionSettings"), TEXT("gc.TimeBetweenPurgingPendingKillObjects"), defaultGCPurgeValue, GEngineIni);
	FString resetGcIntervalCmd = TEXT("gc.TimeBetweenPurgingPendingKillObjects ");
	resetGcIntervalCmd.Append(valueFound ? defaultGCPurgeValue : TEXT("61.1"));
	
	GEngine->Exec(nullptr, *resetGcIntervalCmd);
	UE_LOG(LogTemp, Log, TEXT("End video finished: GC Object purge interval decreased."));
}

void AMissionProgressHandler::TriggerUI() {
	OnRep_UpdateUI(); // unnecessary for anything but SERVER?!
	OnNewObjectiveStarted.Broadcast(NewMissionDetails.Name, NewMissionDetails.Id.Get(""));
}

void AMissionProgressHandler::ClearMissionStateForAllLocalPlayers(ELevelNames level, const FString& missionStateGuid) {
	for (auto* player : InstanceTracker<APlayerCharacter>::GetList(GetWorld())) {
		if (player->IsLocallyControlled()) {
			if (auto* serialize = player->GetCharacterSerializeComponent()) {
				serialize->ClearMissionState(level, missionStateGuid);
			}
		}
	}
}

void AMissionProgressHandler::OnRep_MissionDetails() {

	if (NewMissionDetails.UICountdown > 0) {
		FTimerHandle T1;
		GetWorld()->GetTimerManager().SetTimer(T1, this, &AMissionProgressHandler::TriggerUI, NewMissionDetails.UICountdown, false);
	}
	else
		TriggerUI();

}

void AMissionProgressHandler::FinishedObjectiveTag_Implementation(const FString& tagName) {
	for (ULocalPlayer* localPlayer : GetWorld()->GetGameInstance()->GetLocalPlayers()) {
		if (ABasePlayerController* playerController = Cast<ABasePlayerController>(localPlayer->GetPlayerController(GetWorld()))) {
			auto&& serializeComponent = playerController->GetCharacterSerializeComponent();
			check(serializeComponent->HasProfile());
			if (serializeComponent->HasProfile()) {
				serializeComponent->FinishedObjectiveTag(stringutil::toStdString(tagName));
			}
		}
	}
}

void AMissionProgressHandler::MulticastGameOver_Implementation(ELevelNames level, const FString& missionStateGuid, const FLevelSettings& travelLevelSettings) {
	ClearMissionStateForAllLocalPlayers(level, missionStateGuid);

	static const float WAIT_FOR_UI_COUNTDOWN_DURATION = 5.f;
	MoveToNextLevelDelayed(travelLevelSettings, WAIT_FOR_UI_COUNTDOWN_DURATION);
}

bool AMissionProgressHandler::AllowPlayVideo(const MissionDef& missionDef) {
	auto* videoSource = missionDef.getEndVideo();
	if(!videoSource)
		return false;

	if (ABasePlayerController* playerController = Cast<ABasePlayerController>(GetGameInstance<UDungeonsGameInstance>()->GetUserManager()->GetInitialPlayerController())) {
		if (auto* characterSerialiseComponent = playerController->GetCharacterSerializeComponent())
		{
			//virtual bool GetMediaOption(const FName& Key, bool DefaultValue) const override;
			if (const auto videoMetaData = missionDef.getEndVideoMetadata()) {
				if (videoMetaData.GetValue().ShouldPlayOnce()) {
					if (!characterSerialiseComponent->HasVideoBeenPlayed(*videoSource)) {
						characterSerialiseComponent->SetVideoWasPlayed(*videoSource);
						return true;
					}
					else {
						return false;
					}
				}
			}
		}
	}
	return true;
}

void AMissionProgressHandler::MulticastMissionFinished_Implementation(
	const FMissionFinishedSummary& missionFinishedSummary,
	const FLevelSettings& travelLevelSettings)
{
	mNextLevelSettings = travelLevelSettings;

	// TODO: Use TrialsProvider here instead.
	// online::trials::MarkTrialAsCompleted(trialId);
	auto gameInstance = GetGameInstance<UDungeonsGameInstance>();
	
	const auto& finishedLevelSettings = missionFinishedSummary.levelSettings;
	auto&& missionDef = missions::get(finishedLevelSettings.getLevelName());

	//dont show mission summary for ones that dont require it
	if (missionDef.isShowingVictoryScreen())
	{
		gameInstance->SetMissionFinishedSummary(missionFinishedSummary.GetCopyAdaptedToLocalControllers(GetWorld()));
	}

	if (!HasAuthority()) {
		SetLocalPlayersCompletedMission(finishedLevelSettings);
	}
	if (!gameInstance->IsDedicatedServerInstance()) {
		RewardLocalPlayers(finishedLevelSettings, missionFinishedSummary.GetTrialId());
	}

	mMissionEndedAtSeconds = GetWorld()->GetTimeSeconds();
	mMissionFinishedWasTriggered = true;

	static const float OUTRO_DELAY_MAX = 20.f;
	

	float outroDuration = missionDef.GetVictoryDuration();
	gameInstance->GetAudioMusicManager()->GetCurrentLevelAudioCollection().SyncLoadLevelAudio(); //make sure we have it just in case
	if (USoundCue* VOOutro = gameInstance->GetAudioMusicManager()->GetCurrentLevelAudioCollection().LevelVOOutro.Get()) {
		outroDuration = FMath::Clamp(VOOutro->GetDuration(), outroDuration, OUTRO_DELAY_MAX);
	}

	if (configfile::IsDemo() && !configfile::ShowRewardScreen()) {
		GetWorld()->GetTimerManager().SetTimer(mEndMissionHandle, this, &AMissionProgressHandler::RestartInMenu, outroDuration, false);
		OnShowMissionVictory(outroDuration);
	}
	else {
		if (AllowPlayVideo(missionDef))	{
			ExecuteBeforeVideo();
			PlayMissionEndVideo(missionDef.getEndVideo());
		}
		else {
			if (missionDef.isShowingVictoryScreen())
			{			
				FTimerDelegate endMissionDelegate;
				endMissionDelegate.BindUObject(this, &AMissionProgressHandler::MoveToNextLevelSettings);
				GetWorld()->GetTimerManager().SetTimer(mEndMissionHandle, endMissionDelegate, outroDuration, false);
				OnShowMissionVictory(outroDuration);
			}
			else
			{
				if (Role == ROLE_Authority)
				{
					MulticastInstantMoveToNextLevel(mNextLevelSettings);
				}
			}
		}
	}																				 
	OnMissionFinished();
}


void AMissionProgressHandler::RewardLocalPlayers(const FLevelSettings& levelSettings, const TOptional<FString>& trialId) {
	//D11.KS - All local players should receive the award.
	for (ULocalPlayer* localPlayer : GetWorld()->GetGameInstance()->GetLocalPlayers())
	{
		if (ABasePlayerController* playerController = Cast<ABasePlayerController>(localPlayer->GetPlayerController(GetWorld())))
		{
			if (const auto targetPlayer = playerController->GetControlledPlayerCharacter()) {
				const auto RewardsData = GetRewardItemsForPlayer(targetPlayer, levelSettings); //its now plausable to have levels with zero rewards				
				for (const auto& reward : RewardsData)
				{
					targetPlayer->GetCharacterSerializeComponent()->AddPendingRewardItem(reward);
				}
				if (trialId) {
					targetPlayer->GetCharacterSerializeComponent()->AddTrialCompleted(trialId.GetValue(), levelSettings.getDifficulty());
				}
			}
		}
	}
}

void AMissionProgressHandler::SetLocalPlayersCompletedMission(const FLevelSettings& levelSettings) {
	ClearMissionStateForAllLocalPlayers(levelSettings.getLevelName(), levelSettings.missionState.guid);

	for (auto* player : InstanceTracker<APlayerCharacter>::GetList(GetWorld())) {
		if (player->IsLocallyControlled()) {
			if (auto missionProgressComponent = player->FindComponentByClass<UMissionProgressComponent>()) {
				int tokensCollected = levelSettings.getEmergentDifficulty().GetTotalTokensCollected();
				missionProgressComponent->SetCompletedMission(levelSettings.getDifficulty(), levelSettings.getThreatLevel(), levelSettings.getEndlessStruggle(), levelSettings.getLevelName(), tokensCollected);
			}
			if (auto characterSerialize = player->FindComponentByClass<UCharacterSerializeComponent>()) {
				characterSerialize->IncrementProgressStat(EProgressStat::WIN_MISSIONS);

				if (missions::get(levelSettings.getLevelName()).isHyperMission()) {
					characterSerialize->IncrementProgressStat(EProgressStat::WIN_HYPERMISSIONS);
				}

				const auto endless = levelSettings.getEndlessStruggle();
				//Intentional case-fall-through here!
				switch (levelSettings.getDifficulty()) {
					case EGameDifficulty::Difficulty_3:
						if(endless.Value > 0){
							characterSerialize->IncrementProgressStat(EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS);
							for (auto tier : endlesstruggle::tier::getAllHighestToLowest()) {
								if (endless >= tier->unlockedEndlessStruggle()) {
									characterSerialize->IncrementProgressStat(tier->progressStat());
								}
							}
						}
						characterSerialize->IncrementProgressStat(EProgressStat::WIN_MISSIONS_APOCALYPSE);
					case EGameDifficulty::Difficulty_2:
						characterSerialize->IncrementProgressStat(EProgressStat::WIN_MISSIONS_ADVENTURE);
					case EGameDifficulty::Difficulty_1:
						characterSerialize->IncrementProgressStat(EProgressStat::WIN_MISSIONS_DEFAULT);
					default:
						break;
				}
			}
		}
	}			
}

TArray<FRewardData> AMissionProgressHandler::GetRewardItemsForPlayer(APlayerCharacter* targetPlayer, const FLevelSettings& levelSettings) const{
	TArray<FRewardData> Items;
	const auto& mission = missions::get(levelSettings.getLevelName());
	const auto rewards = FloatWeighedRandom::getWeightedRandomItemOrEmpty(mission.rewards().itemChances()).GetValue();
	auto predRarity = createPredRarity(rewards);

	const game::item::drop::DropGenerationInput rewardData(targetPlayer, predRarity.pred, predRarity.rarityChance, itemgen::ItemSource::MissionReward);

	if (TOptional<FInventoryItemData> levelCompletionReward = generateDroppedItem(GetWorld(), rewardData)) {
		if (auto count = mission.overrideRewardCount()) {
			levelCompletionReward->OverrideStoreCount = count.GetValue();
		}
		Items.Emplace(ERewardType::LevelCompletion, levelCompletionReward.GetValue());
	}

	//Emergent Difficulty
	const float tokensCollected = levelSettings.getEmergentDifficulty().GetTotalTokensCollected();
	if (tokensCollected > 0) {
		const float UniqueRarityChance = FMath::Clamp(tokensCollected / rewardquery::GetMaxEmergentDifficultyCollectedTokens() * mEmergencyUniqueCap, 0.f, 1.f);
		const float RestRarityChance = (1 - UniqueRarityChance) / 2;

		const game::item::drop::DropGenerationInput EmergentDifficultyRewardData(targetPlayer, itemgen::predicates::Gear(), FRareItemChance({
				{EItemRarity::Common, RestRarityChance},
				{EItemRarity::Rare, RestRarityChance},
				{EItemRarity::Unique, UniqueRarityChance}
			})
			, itemgen::ItemSource::RaidCaptainMissionReward);

		if (TOptional<FInventoryItemData> emergentDifficultyReward = generateDroppedItem(GetWorld(), EmergentDifficultyRewardData)) {
			Items.Emplace(ERewardType::EmergentDifficulty, emergentDifficultyReward.GetValue());
		}
	}
	return Items;
}

void AMissionProgressHandler::MulticastObjectiveEventLocation_Implementation(const FObjectiveEventLocation& location, float duration) {
	ObjectiveEventHandler.queuePanEvent(location, duration);
	ObjectiveEventHandler.tryStart(*GetWorld(), { true });
}

void AMissionProgressHandler::MulticastInstantMoveToNextLevel_Implementation(const FLevelSettings& travelLevelSettings) {
	MoveToNextLevel(travelLevelSettings);
}

void AMissionProgressHandler::SetObjectiveEventLocation(const FObjectiveEventLocation& location, float duration) {
	MulticastObjectiveEventLocation(location, duration);
}

int AMissionProgressHandler::GetMissionProgressionCount() const {
	return mMissionProgressCount;
}

bool AMissionProgressHandler::WasMissionFinishedTriggeredOnce() const
{
	return mMissionFinishedWasTriggered;
}

void AMissionProgressHandler::CameraPanningStarted()
{
	OnCameraPanStarted.Broadcast();

}

void AMissionProgressHandler::CameraPanningCompleted()
{
	OnCameraPanDone.Broadcast();

}

void AMissionProgressHandler::RepNotifyInternal() {
	if (Role == ROLE_Authority) {
		OnRep_MissionDetails();
	}
}

void AMissionProgressHandler::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMissionProgressHandler, IsVisible);	
	DOREPLIFETIME(AMissionProgressHandler, NewMissionDetails);
	DOREPLIFETIME(AMissionProgressHandler, mMissionProgressCount);
}

namespace game { namespace objective {

EObjectiveDisplayMode displayModeFromString(const FString& s) {
	if (s == "InitLevel") {
		return EObjectiveDisplayMode::InitLevel;
	}
	if (s == "MainObjective") {
		return EObjectiveDisplayMode::NewMainObjective;
	}
	return EObjectiveDisplayMode::NewSubobjective;
}

FMissionDetails makeMissionDetails(const Objective& obj) {
	FMissionDetails out;

	const auto& info = obj.info();
	out.Name = info.name;
	out.Description = info.description;
	out.DisplayMode = displayModeFromString(info.displayMode);
	out.ObjectiveIndex = info.objectiveIndex;
	out.Id = info.id;
	out.AlwaysShow = info.alwaysShow;
	out.UICountdown = obj.uiDelayTime();

	auto completion = obj.completion();
	out.CountMax = completion.max();
	out.Count = completion.current();

	return out;
}

}}
