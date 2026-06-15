#include "Dungeons.h"
#include "MissionProgressComponent.h"
#include "ItemStashComponent.h"
#include "DungeonsGameInstance.h"
#include "DungeonsUserManagement.h"
#include "game/component/CharacterSerializeComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/EndGame/EndGameContent.h"
#include "game/EndGame/EndGameContentDefs.h"
#include "game/mission/info/MissionChancesUtil.h"
#include "game/mission/MissionSelection.h"
#include "game/mission/MissionDefs.h"
#include "game/mission/hyper/HyperMissions.h"
#include "game/mission/offerings/MissionOfferingsUtil.h"
#include "game/mission/request/MissionRequestUtil.h"
#include "game/mission/start/StartMissionStatusUtil.h"
#include "game/mission/difficulty/MissionDifficulty.h"
#include "game/difficulty/endless/EndlessStruggleTier.h"
#include "game/difficulty/endless/EndlessStruggleTiers.h"
#include "game/merchant/MerchantsUtil.h"
#include "game/realms/RealmDefs.h"
#include "game/GameBP.h"
#include "game/difficulty/DifficultyUtil.h"
#include "game/dlc/DLCDefs.h"
#include "game/util/UnlockKeyUtils.h"
#include "game/util/ValueFormat.h"
#include "CommonTypes.h"
#include "online/sessions/OnlineUtil.h"
#include <algorithm>
#include <Themida/Anticheat.hpp>
#include "ClientEventHub.h"
#include "WalletComponent.h"


static bool IsMissionTutorial(ELevelNames LevelName) {
	return missions::get(LevelName).isTutorial();
}

UMissionProgressComponent::UMissionProgressComponent() {
}

void UMissionProgressComponent::OnLocalPawnPossessed() {
	DeserializeSaveState();
	SetDifficultyUnlocked(GetHighestDifficultyWhichShouldBeUnlocked());
	SetThreatLevelUnlocked(GetHighestThreatLevelWhichShouldBeUnlocked());
	GetDungeonsGameInstance()->GetEntitlementsRepository()->OnEntitlementsProvided.AddUObject(this, &UMissionProgressComponent::OnEntitlementsProvided);	
}

void UMissionProgressComponent::OnEntitlementsProvided(const TArray<FEntitlement>& entitlements) {
	mUnlockedMissionsSetCache.Reset();
	OnMissionsChanged.Broadcast();
}

UDungeonsGameInstance* UMissionProgressComponent::GetDungeonsGameInstance() const {
	return GetWorld()->GetGameInstance<UDungeonsGameInstance>();
}


void UMissionProgressComponent::BeginPlay() {
	Super::BeginPlay();

	if (auto* game = actorquery::getFirstActor<AGameBP>(GetWorld())) {
		game->OnTrialMissionsUpdated.AddUObject(this, &UMissionProgressComponent::SetTrialMissions);
	}
}

/*
Logic
*/
ANTICHEAT_NO_OPTIMIZATION_BEGIN
bool UMissionProgressComponent::IsDLCPlayable(EDLCName dlc) const {

	ANTICHEAT_OBFUSCATE_BEGIN
		
	if (const auto* dlcDef = dlc::getChecked(dlc)) {
		if (dlcDef->IsDisabled()) {
			return false;
		}

		if (auto requiredRealm = dlcDef->GetRequiredRealm()) {
			if (!IsRealmUnlocked(requiredRealm.GetValue())) {
				return false;
			}
		}

		return IsDLCOwned(dlc);
	}

	return false;

	ANTICHEAT_OBFUSCATE_END
}
ANTICHEAT_NO_OPTIMIZATION_END

ANTICHEAT_NO_OPTIMIZATION_BEGIN
bool UMissionProgressComponent::IsDLCOwned(EDLCName dlc) const {

	ANTICHEAT_OBFUSCATE_BEGIN
		
	if (const auto* dlcDef = dlc::getChecked(dlc)) {
#if !UE_BUILD_SHIPPING
		if (bUnlockAllDLC) {
			return true;
		}
#endif		
		return dlcDef->IsUnlockedByEntitlements(GetWorld()) && dlcDef->IsReleased();
	}

	return false;

	ANTICHEAT_OBFUSCATE_END
}
ANTICHEAT_NO_OPTIMIZATION_END

TArray<EDLCName> UMissionProgressComponent::GetOwnedDLCs() const {
	return algo::copy_if(dlc::getAllEnabled(), RETLAMBDA(IsDLCOwned(it)));
}

bool UMissionProgressComponent::IsRealmUnlocked(ERealmName Realm) const {

#if !UE_BUILD_SHIPPING
	//General overrides.
	if (bUnlockAllRealms) {
		return true;
	}
#endif

	return !URealmDefs::IsRealmLocked(Realm);
}

bool UMissionProgressComponent::IsMissionCompletedAtAnyDifficulty(ELevelNames requiredLevel) const{
	return IsMissionDifficultyCompleted(difficultyquery::First, requiredLevel);
}

bool UMissionProgressComponent::IsMissionUnlocked(EGameDifficulty Difficulty, ELevelNames LevelName) const {	
	auto missionDef = missions::getChecked(LevelName);
	if (!missionDef) {
		return false;
	}

	if (missionDef->isDisabled()) {
		return false;
	}

	if (missionDef->isTutorial() && online::isOnlineSession()) {
		return false;
	}

#if !UE_BUILD_SHIPPING
	//General overrides.
	if (bUnlockAllMissions) {
		return true;
	}
#endif

	if (auto requiredDlc = missionDef->getRequiredDLC()) {
		if (!IsDLCPlayable(requiredDlc.GetValue())) {
			return false;
		}
	}

	if (LevelName == ELevelNames::enderwilds) {
		return GetCharacterSerializeComponent()->HasUsedPortal();
	}

	if (auto type = missionDef->getRequiredEndGameContent()) {
		auto serialize = GetCharacterSerializeComponent();
		if (!endgamecontent::isUnlocked(serialize, type.GetValue())) {
			return false;
		}
	}

	//Otherwise - only unlock missions in difficulties we have unlocked.
	if (IsDifficultyUnlocked(Difficulty)) {
		const bool allRequirementsMet = [&]() {
				
			if (missionDef->isSecretMission()) {
				if (!HasUnlockedBonusMission(LevelName)) {
					return false;
				}
			}
				
			for (auto requiredLevel : missionDef->getRequiredMissions()) {
				//To unlock a mission - all required missions need to be completed.
				if (!IsMissionCompleted(requiredLevel)) {
					return false;
				}
			}

			if (auto requiredUnlockedDifficulty = missionDef->getRequiredUnlockedDifficulty()) {
				if (!IsDifficultyUnlocked(requiredUnlockedDifficulty.GetValue())) {
					return false;
				}
			}

			//Difficulty needs to be valid for this mission
			return missionDef->isValidDifficulty(Difficulty);
		}();

		return allRequirementsMet;
	}

	return false;
}

bool UMissionProgressComponent::IsMissionRevealed(EGameDifficulty Difficulty, ELevelNames LevelName) const {
	//General overrides.
	auto missionDef = missions::getChecked(LevelName);
	if (!missionDef) {
		return false;
	}

	if (missionDef->isDisabled()) {
		return false;
	}

	if (bUnlockAllMissions) {
		return true;
	}

	if (auto requiredDlc = missionDef->getRequiredDLC()) {
		if (!IsDLCPlayable(requiredDlc.GetValue())) {
			return false;
		}
	}

	if (missionDef->isTutorial()){
		if (online::isOnlineSession() || !missionDef->isValidDifficulty(Difficulty)) {
			return false;
		}
	}

	//Unlocked missions are always revealed
	if (IsMissionUnlocked(Difficulty, LevelName)) {
		return true;
	}

	//Otherwise - only reveal missions in difficulties we have unlocked.
	if (IsDifficultyUnlocked(Difficulty)) {
		const bool allRequirementsMet = [&]() {			
			if (missionDef->isSecretMission()) {
				//To reveal a bonus mission, it needs to be unlocked.
				if (HasUnlockedBonusMission(LevelName)) {
					return true;
				}

				if (auto revealedByMission = missionDef->getSecretUnlockFoundInMission()) {
					if (IsMissionCompleted(revealedByMission.GetValue())) {
						return true;
					}
				}

				return false;
			} 

			const auto& requiredMissions = missionDef->getRequiredMissions();
			if (requiredMissions.Num() > 0 && algo::none_of( requiredMissions, RETLAMBDA(IsMissionUnlocked(difficultyquery::First, it)))) {
				return false;
			}

			return true;
		}();

		return allRequirementsMet;
	}

	return false;
}

//A mission is completed when it is completed at any difficulty
bool UMissionProgressComponent::IsMissionCompleted(ELevelNames LevelName) const {
	return IsMissionCompletedAtAnyDifficulty(LevelName);
}

bool UMissionProgressComponent::IsDifficultyCompleted(EGameDifficulty Difficulty) const {
	const auto& missionDef = missions::get(ELevelNames::obsidianpinnacle);
	const EThreatLevel minimalThreatLevel = missionDef.getMinimalThreatLevel().Get(EThreatLevel::Invalid);
	return IsMissionDifficultyAndThreatLevelCompleted(Difficulty, minimalThreatLevel, FEndlessStruggle(), missionDef.level());
}

bool UMissionProgressComponent::ShouldDifficultyBeUnlocked(EGameDifficulty Difficulty) const {
	//Default unlocks
	if (difficultyquery::isUnlockedByDefault(Difficulty)) {
		return true;
	}

	switch (Difficulty) {	
		case EGameDifficulty::Difficulty_2:
		case EGameDifficulty::Difficulty_3:
			/*Iron/Diamond difficulty unlock logic:
				"Beat the Arch-Illager on at least the minimum threat level"
				
				Should be coupled with UDifficultyUtil::GetUnlockText and UDifficultyUtil::GetUnlockRequirementText
			*/
			return IsDifficultyCompleted(difficultyquery::getPreviousDifficulty(Difficulty).Get(EGameDifficulty::Invalid));
		default:
			return false;
	}
}


EGameDifficulty UMissionProgressComponent::GetHighestDifficultyWhichShouldBeUnlocked() const {
	for (int i = enum_cast(EGameDifficulty::ENUM_COUNT) - 1; i >= EGameDifficultyFirstIndex; --i) {
		EGameDifficulty diff = static_cast<EGameDifficulty>(i);
		if (ShouldDifficultyBeUnlocked(diff)) {
			return diff;
		}
	}
	return EGameDifficulty::Invalid;
}


//Threat level unlock logic

bool UMissionProgressComponent::ShouldThreatLevelBeUnlocked(EThreatLevel ThreatLevel) const {
	//Default unlocks
	if (threatquery::isUnlockedByDefault(ThreatLevel)) {
		return true;
	}

	//If you have completed a previous threat level - the higher one should be unlocked	
	EThreatLevel requiredThreatLevel = threatquery::getCompletedForUnlocked(ThreatLevel);
	if (requiredThreatLevel != EThreatLevel::Invalid) {
		for (auto& progress : GetCompletedMissions()) {
			if (progress.completedThreatLevel >= requiredThreatLevel) {
				return true;
			}
		}
	}
	return false;
}


EThreatLevel UMissionProgressComponent::GetHighestThreatLevelWhichShouldBeUnlocked() const {
	for (int i = enum_cast(EThreatLevel::ENUM_COUNT) - 1; i >= EThreatLevelFirstIndex; --i) {
		EThreatLevel threatLevel = static_cast<EThreatLevel>(i);
		if (ShouldThreatLevelBeUnlocked(threatLevel)) {
			return threatLevel;
		}
	}
	return EThreatLevel::Invalid;
}

void UMissionProgressComponent::RefreshHighestCompletedMissionDifficulty() {	
	auto missionProgress = GetCompletedMissions();
	missionProgress.Sort([](const FMissionProgress& a, const FMissionProgress& b) {
		return FMissionDifficulty::from(b) < FMissionDifficulty::from(a);
	});

	if (missionProgress.IsValidIndex(0)) {
		mHighestCompletedMissionDifficultyCache = FMissionDifficulty::from(missionProgress[0]);
	} else {
		mHighestCompletedMissionDifficultyCache.Reset();
	}
}
const TOptional<FMissionDifficulty>& UMissionProgressComponent::GetHighestCompletedMissionDifficulty() const {
	return mHighestCompletedMissionDifficultyCache;
}

/*
 Set Logic
 */

void UMissionProgressComponent::SetCompletedMission(EGameDifficulty difficulty, EThreatLevel threatLevel, FEndlessStruggle endlessStruggle, ELevelNames levelName, int TokensCollected) {
	auto&& mp = GetOrCreateEditableMissionProgress(levelName);
	
	const FMissionDifficulty currentAt = FMissionDifficulty{ levelName, mp.completedDifficulty, mp.completedThreatLevel, mp.completedEndlessStruggle };
	const FMissionDifficulty completedAt = FMissionDifficulty{ levelName, difficulty, threatLevel, endlessStruggle };

	if (currentAt < completedAt) {
		mp.completedDifficulty = difficulty;
		mp.completedThreatLevel = threatLevel;
		mp.completedEndlessStruggle = endlessStruggle;

		SerializeSaveState();
		mUnlockedMissionsSetCache.Reset();
		HighestCompletedDifficultyStats.Reset();
		OnMissionsCompletedChanged.Broadcast();
		SetDifficultyUnlocked(GetHighestDifficultyWhichShouldBeUnlocked());
		SetThreatLevelUnlocked(GetHighestThreatLevelWhichShouldBeUnlocked());
	}
	
	
	EExtraChallenge extraChallenge = EExtraChallenge::NoExtraChallenge;
	bool hyperMission = false;
	ELevelVariationType levelVariation = ELevelVariationType::none;
	if (const auto* game = actorquery::getGame(GetWorld())) {
		extraChallenge = game->settings().difficulty.extraChallenge();
		levelVariation = game->missionDef().levelVariation();
		hyperMission = game->missionDef().isHyperMission();
	}

	if (auto* hub = GetCharacterOwner()->GetClientEventHubComponent()) {
		hub->MissionFinished(levelName, difficulty, endlessStruggle, extraChallenge, hyperMission, levelVariation, TokensCollected);
	}
}

void UMissionProgressComponent::SetDifficultyUnlocked(EGameDifficulty Difficulty) {
	if (Difficulty > mDifficultyProgress.unlocked) {
		mDifficultyProgress.unlocked = Difficulty;
		mMapUIState.selectedDifficulty = Difficulty;
		OnDifficultyUnlockedChanged.Broadcast();
		SerializeSaveState();
		mUnlockedMissionsSetCache.Reset();
	}
}

void UMissionProgressComponent::SetThreatLevelUnlocked(EThreatLevel ThreatLevel) {
	if (ThreatLevel > mThreatLevelProgress.unlocked) {
		mThreatLevelProgress.unlocked = ThreatLevel;
		OnThreatLevelUnlockedChanged.Broadcast();
		SerializeSaveState();
		mUnlockedMissionsSetCache.Reset();
	}
}

void UMissionProgressComponent::SetTrialMissions() const {
	OnMissionsChanged.Broadcast();
}

void UMissionProgressComponent::UnlockBonusMissionForAllLocalPlayers(UObject* WorldContextObject, ELevelNames LevelName) {
	for (auto* player : InstanceTracker<APlayerCharacter>::GetList(WorldContextObject->GetWorld())) {
		if (player->IsLocallyControlled()) {
			player->GetMissionProgressComponent()->UnlockBonusMission(LevelName);
		}
	}
}

void UMissionProgressComponent::UnlockBonusMissionForAllPlayers(UObject* WorldContextObject, ELevelNames level) {
	for (auto it = WorldContextObject->GetWorld()->GetPlayerControllerIterator(); it; ++it) {
		if (auto* controller = Cast<ABasePlayerController>(it->Get())) {
			if (controller->HasAuthority()) {
				if (auto* player = controller->GetControlledPlayerCharacter()) {
					player->ClientUnlockBonusMission(level);
				}
			}
		}
	}
}

void UMissionProgressComponent::UnlockBonusMission(ELevelNames LevelName) {
	check(missions::get(LevelName).isSecretMission());
	mUnlockedSecretMissions.Add(LevelName);	
	SerializeSaveState();
}

bool UMissionProgressComponent::HasUnlockedBonusMission(ELevelNames LevelName) const {
	auto& missionDef = missions::get(LevelName);
	checkf(missionDef.isSecretMission(), TEXT("%s is not a secret mission, probably a mistake to check this"), *GetEnumValueToString(LevelName));
	if (auto requiredSecretMissionUnlock = missionDef.getNeededSecretMissionUnlock()) {
		return mUnlockedSecretMissions.Contains(requiredSecretMissionUnlock.GetValue());
	}
	return true;
}







/*
Animations
*/

int32 UMissionProgressComponent::GetNumCompletedMissionsAtDifficulty(EGameDifficulty difficulty) const {
	return algo::count_if(mCompletedMissions, RETLAMBDA(it.completedDifficulty >= difficulty));
}

int32 UMissionProgressComponent::GetNumCompletedMissionsInSet(TSet<ELevelNames> missions, TOptional<EGameDifficulty> difficulty, TOptional<EThreatLevel> threatlevel) const {
	return algo::count_if(missions.Array(), RETLAMBDA(IsMissionDifficultyAndThreatLevelCompleted(difficulty.Get(difficultyquery::First), threatquery::First, FEndlessStruggle{0}, it)));
}

bool UMissionProgressComponent::GetHasCompletedMissionAtDifficulty(EGameDifficulty difficulty, ELevelNames levelName) const {
	return algo::any_of(mCompletedMissions, RETLAMBDA(it.levelName == levelName && it.completedDifficulty >= difficulty));
}

const TArray<FMissionProgress>& UMissionProgressComponent::GetCompletedMissions() const {
	return mCompletedMissions;
}




bool UMissionProgressComponent::IsDifficultyAnnounced(EGameDifficulty Difficulty) const {
	return mDifficultyProgress.announced >= Difficulty;
}

bool UMissionProgressComponent::IsDifficultyUnlocked(EGameDifficulty Difficulty) const {
	return mDifficultyProgress.unlocked >= Difficulty || bUnlockAllMissions;
}


void UMissionProgressComponent::SetDifficultyAnnounced(EGameDifficulty Difficulty) {
	if (Difficulty > mDifficultyProgress.announced) {
		mDifficultyProgress.announced = Difficulty;
		SerializeSaveState();
	}
}

void UMissionProgressComponent::SetContentAnnounced(EEndGameContentType Type) {
	if (!mEndGameContentProgress.alreadyAnnouncedUnlockedContent.Contains(Type))
	{
		mEndGameContentProgress.alreadyAnnouncedUnlockedContent.Add(Type);
		SerializeSaveState();
	}
}

TArray<EEndGameContentType> UMissionProgressComponent::GetUnannouncedEndGameContent() const {
	return endgamecontent::unlockedUnannouncedTypes(GetCharacterSerializeComponent());
}

EGameDifficulty UMissionProgressComponent::GetUnannouncedUnlockedDifficulty() const {
	if (mDifficultyProgress.announced < mDifficultyProgress.unlocked && !difficultyquery::isUnlockedByDefault(mDifficultyProgress.unlocked)) {
		return mDifficultyProgress.unlocked;
	}
	return EGameDifficulty::Invalid;
}


bool UMissionProgressComponent::HasNeverSelectedDifficulty() const {
	return mDifficultyProgress.unlocked > mDifficultyProgress.selected;
}

void UMissionProgressComponent::OnDifficultySelected(EGameDifficulty Difficulty) {
	if (mDifficultyProgress.selected < Difficulty) {
		mDifficultyProgress.selected = Difficulty;
	}
}

EThreatLevel UMissionProgressComponent::GetMissionValidThreatLevel(ELevelNames LevelName, EGameDifficulty Difficulty, EThreatLevel ThreatLevel) const
{
	return missions::get(LevelName).getClampedThreatLevel(ThreatLevel);
}


bool UMissionProgressComponent::IsMissionThreatLevelValid(ELevelNames LevelName, EThreatLevel ThreatLevel) const {
	return missions::get(LevelName).isValidThreatLevel(ThreatLevel);
}

bool UMissionProgressComponent::IsMissionThreatLevelTooLow(ELevelNames LevelName, EThreatLevel ThreatLevel) const {
	return missions::get(LevelName).getClampedThreatLevel(ThreatLevel) > ThreatLevel;
}

bool UMissionProgressComponent::IsMissionThreatLevelTooHigh(ELevelNames LevelName, EThreatLevel ThreatLevel) const {
	return missions::get(LevelName).getClampedThreatLevel(ThreatLevel) < ThreatLevel;
}

bool UMissionProgressComponent::IsAnyEndlessStruggleUnlocked(EGameDifficulty Difficulty) const {
	return GetHighestUnlockedEndlessStruggleTier(Difficulty) != nullptr;
}

FEndlessStruggle UMissionProgressComponent::GetHighestCompletedEndlessStruggle() const {
	if (const auto& highest = GetHighestCompletedMissionDifficulty()) {
		return highest->endlessStruggle;
	}
	return FEndlessStruggle::ZERO;
}

FEndlessStruggle UMissionProgressComponent::GetHighestUnlockedEndlessStruggle(EGameDifficulty Difficulty) const {
	if (auto highestTier = GetHighestUnlockedEndlessStruggleTier(Difficulty)) {
		return highestTier->unlockedEndlessStruggle();
	}
	return FEndlessStruggle::ZERO;
}

bool UMissionProgressComponent::IsMissionEndlessStruggleUnlocked(EGameDifficulty Difficulty, FEndlessStruggle EndlessStruggle) const {
	return EndlessStruggle <= GetHighestUnlockedEndlessStruggle(Difficulty);
}

FEndlessStruggle UMissionProgressComponent::GetMissionValidEndlessStruggle(ELevelNames LevelName, EGameDifficulty Difficulty, EThreatLevel ThreatLevel, FEndlessStruggle EndlessStruggle) const {
	if (IsAnyEndlessStruggleUnlocked(Difficulty) && ThreatLevel >= threatquery::Last) {
		return std::min(EndlessStruggle, GetHighestUnlockedEndlessStruggle(Difficulty));
	} else {
		return FEndlessStruggle::ZERO;
	}
}

bool UMissionProgressComponent::IsThreatLevelUnlocked(EThreatLevel ThreatLevel) const {
	return mThreatLevelProgress.unlocked >= ThreatLevel || bUnlockAllMissions;
}

bool UMissionProgressComponent::IsThreatLevelAnnounced(EThreatLevel ThreatLevel) const {
	return mThreatLevelProgress.announced >= ThreatLevel;
}

void UMissionProgressComponent::SetThreatLevelAnnounced(EThreatLevel ThreatLevel) {
	if (ThreatLevel > mThreatLevelProgress.announced) {
		mThreatLevelProgress.announced = ThreatLevel;
		SerializeSaveState();
	}
}

EThreatLevel UMissionProgressComponent::GetUnannouncedUnlockedThreatLevel() const {
	if (mThreatLevelProgress.announced < mThreatLevelProgress.unlocked && !threatquery::isUnlockedByDefault(mThreatLevelProgress.unlocked)) {
		return mThreatLevelProgress.unlocked;
	}
	return EThreatLevel::Invalid;
}








bool UMissionProgressComponent::IsMissionDifficultyAndThreatLevelCompleted(EGameDifficulty Difficulty, EThreatLevel ThreatLevel, FEndlessStruggle EndlessStruggle, ELevelNames LevelName) const {
	if (auto Completion = GetMissionProgress(LevelName)) {		
		return FMissionDifficulty{ LevelName, Completion->completedDifficulty, Completion->completedThreatLevel, Completion->completedEndlessStruggle.Value }
			>= FMissionDifficulty{ LevelName, Difficulty, ThreatLevel, EndlessStruggle.Value };
	}
	return false;
}

 




namespace missionprogress {
	TArray<const MissionDef*> allMissionsInDifficultyOfVariationType(EGameDifficulty difficulty, ELevelVariationType variationType) {
		return missions::getAllCurrentlyEnabledMatching(RETLAMBDA(it.levelVariation() == variationType && it.isValidDifficulty(difficulty)));
	}

	static bool MissionSelectOrderSort(const MissionDef& a, const MissionDef& b) {
		return a.getSelectOrder() < b.getSelectOrder();
	}

	TArray<const MissionDef*> allSortedMissionsInDifficulty(EGameDifficulty difficulty) {
		auto filteredMissions = missions::getAllCurrentlyEnabledMatching(RETLAMBDA(it.isValidDifficulty(difficulty)));
		filteredMissions.Sort(MissionSelectOrderSort);
		return filteredMissions;
	}

	TArray<ELevelNames> allSortedMissionNamesInDifficulty(EGameDifficulty difficulty) {
		return algo::map_tarray(allSortedMissionsInDifficulty(difficulty), RETLAMBDA(it->level()));
	}
}


int UMissionProgressComponent::GetTotalMissionsInDifficultyOfVariationType(EGameDifficulty difficulty, ELevelVariationType variationType) const
{
	return missionprogress::allMissionsInDifficultyOfVariationType(difficulty, variationType).FilterByPredicate([&](const MissionDef* mission) {
		return IsMissionUnlocked(difficulty, mission->level());
	}).Num();
}

int UMissionProgressComponent::GetCompletedMissionsInDifficultyOfVariationType(EGameDifficulty difficulty, ELevelVariationType variationType) const
{	
	return missionprogress::allMissionsInDifficultyOfVariationType(difficulty, variationType).FilterByPredicate([&](const MissionDef* mission) {
		return IsMissionDifficultyCompleted(difficulty, mission->level());
	}).Num();	
}

bool UMissionProgressComponent::DoesMissionContainPreviouslyNotFoundItemTypes(EGameDifficulty difficulty, ELevelNames levelName) const {	
	FMissionDifficulty missionDifficulty = { levelName, difficulty, EThreatLevel::Threat_1 };
	return algo::any_of(UMissionChancesUtil::GetMapLootItemTypeChances(missionDifficulty), RETLAMBDA(!HasPreviouslyFoundItemType(it.ItemId)))
		|| algo::any_of(UMissionChancesUtil::GetMapRewardItemTypeChances(missionDifficulty), RETLAMBDA(!HasPreviouslyFoundItemType(it.ItemId)));
}

bool UMissionProgressComponent::DoesMissionHaveNewCapturedMerchants(EGameDifficulty difficulty, ELevelNames levelName) const {
	return IsMissionUnlocked(difficulty, levelName)
		&& UMerchantsUtil::IsMerchantLockedForLevel(GetOwner(), levelName);
}

bool UMissionProgressComponent::DoesMissionHaveNewSecretLocations(EGameDifficulty difficulty, ELevelNames levelName) const {
	if (IsMissionUnlocked(difficulty, levelName) && IsMissionCompleted(levelName)) {
		return algo::any_of(missionprogress::allSortedMissionNamesInDifficulty(difficulty),
			RETLAMBDA(
				IsMissionRevealed(difficulty, it)
				&& missions::get(it).isSecretUnlockFoundInMission(levelName)
				&& !HasUnlockedBonusMission(it)
			));						
	}
	return false;
}

bool UMissionProgressComponent::DoesMissionHaveNewEnderEye(EGameDifficulty difficulty, ELevelNames levelName) const
{
	if (IsDLCOwned(EDLCName::TheEnd) && IsMissionUnlocked(difficulty, levelName)) {
		const auto& strongholdData = GetCharacterSerializeComponent()->GetStrongholdData();

		switch (levelName)
		{
		case ELevelNames::creeperwoods:    return strongholdData.AllowsPickup(EEyeOfEnderType::DEEPWOOD_BROOK);
		case ELevelNames::pumpkinpastures: return strongholdData.AllowsPickup(EEyeOfEnderType::OLD_TOWN_HALL);
		case ELevelNames::soggyswamp:      return strongholdData.AllowsPickup(EEyeOfEnderType::SUNKEN_WATCHTOWER);
		case ELevelNames::cacticanyon:     return strongholdData.AllowsPickup(EEyeOfEnderType::THE_UNDERCROFT);
		case ELevelNames::deserttemple:    return strongholdData.AllowsPickup(EEyeOfEnderType::THE_TRIAL_OF_THE_NAMELESS);
		case ELevelNames::highblockhalls:  return strongholdData.AllowsPickup(EEyeOfEnderType::HIGHBLOCK_HIDEAWAY);
		default:
			return false;
		}
	}
	return false;
}

bool UMissionProgressComponent::IsMissionNew(EGameDifficulty Difficulty, ELevelNames LevelName) const {
	return IsMissionUnlocked(Difficulty, LevelName)
		&& !IsMissionDifficultyCompleted(Difficulty, LevelName)
		&& !IsMissionTutorial(LevelName);
}

bool UMissionProgressComponent::IsMissionDone(EGameDifficulty Difficulty, ELevelNames LevelName) const {
	return 
		( (IsMissionTutorial(LevelName) && IsMissionCompletedAtAnyDifficulty(LevelName)) || IsMissionDifficultyCompleted(Difficulty, LevelName) )
		&& IsMissionUnlocked(Difficulty, LevelName);
}

bool UMissionProgressComponent::DoesMissionHaveNewRewards(EGameDifficulty Difficulty, ELevelNames LevelName) const {
	return IsMissionUnlocked(Difficulty, LevelName) 
		&& IsMissionCompleted(LevelName)
		&& DoesMissionContainPreviouslyNotFoundItemTypes(Difficulty, LevelName)
		&& !IsMissionTutorial(LevelName);
}

FStartMissionStatus UMissionProgressComponent::QueryStartMissionStatus(const FMissionRequest& missionRequest) const {
	const auto& missionState = missionRequest.getMissionState();
	auto& missionDef = missions::get(missionState.getLevelName());
	if (IsMissionThreatLevelTooLow(missionState.getLevelName(), missionState.getThreatLevel())) {
		return FStartMissionStatus::fail({
			NSLOCTEXT("", "missioninspector_tooeasy", "Below Minimum Difficulty"),
			FText::Format(NSLOCTEXT("MissionProgress", "ThisMissionRequiresAtLeastXToStart", "Minimum required difficulty for mission is {0}"), UDifficultyUtil::getDifficultyThreatLevelDisplayName(missionState.getDifficulty(), missionDef.getMinimalThreatLevel().Get(missionState.getThreatLevel())))
			});
	}
	if (IsMissionThreatLevelTooHigh(missionState.getLevelName(), missionState.getThreatLevel())) {
		return FStartMissionStatus::fail({
			NSLOCTEXT("", "missioninspector_toohard", "Above Maximum Difficulty"),
			FText::Format(NSLOCTEXT("MissionProgress", "ThisMissionRequiresAtMostXToStart", "Maximum difficulty allowed for mission is {0}"), UDifficultyUtil::getDifficultyThreatLevelDisplayName(missionState.getDifficulty(), missionDef.getMaximalThreatLevel().Get(missionState.getThreatLevel())))
			});
	}
	if (GetHighestUnlockedEndlessStruggle(missionState.getDifficulty()) < missionState.getEndlessStruggle()) {
		auto* tier = endlesstruggle::tier::getUnlockFor(missionState.getEndlessStruggle());
		return FStartMissionStatus::fail({
			FText::Format(NSLOCTEXT("MissionProgress", "xDifficultyLockedTemplate", "{0} Locked"), UDifficultyUtil::getDifficultyEndlessStruggleDisplayName(missionState.getDifficulty(), missionState.getEndlessStruggle())),
			tier ? tier->GetUnlockText() : FText::GetEmpty(),
			tier ? tier->GetProgressText(this) : FText::GetEmpty()
			});
	}

	if (!missionDef.isThreatLevelUnlockedByDefault() && !IsThreatLevelUnlocked(missionState.getThreatLevel())) {
		return FStartMissionStatus::fail({
			FText::Format(NSLOCTEXT("MissionProgress", "xDifficultyLockedTemplate", "{0} Locked"), UDifficultyUtil::getDifficultyThreatLevelDisplayName(missionState.getDifficulty(), missionState.getThreatLevel())),
			FText::Format(NSLOCTEXT("MissionProgress", "completeAnyMissionOnXDifficultyToUnlock", "Complete any mission on {0} to Unlock"), UDifficultyUtil::getDifficultyThreatLevelDisplayName(missionState.getDifficulty(), threatquery::getCompletedForUnlocked(missionState.getThreatLevel())))
			});
	}
	if (!ShouldDifficultyBeUnlocked(missionState.getDifficulty())) {
		return FStartMissionStatus::fail({
			NSLOCTEXT("MissionProgress", "missioninspector_difficultylocked", "Difficulty Locked"),
			UDifficultyUtil::GetUnlockText(missionState.getDifficulty())
			});
	}
	if (missionDef.isTrial() && HasCompletedCurrentTrialAtDifficulty(missionState.getLevelName(), missionState.getDifficulty())) {
		return FStartMissionStatus::fail({
			FText::Format(NSLOCTEXT("MissionProgress", "AlreadyCompletedOnx", "Already Completed On {0}"), UDifficultyUtil::getDifficultyDisplayName(missionState.getDifficulty())),
			NSLOCTEXT("MissionProgress", "AlreadyCompletedOnDifficulty_remedy", "Change difficulty or come back at a later time")
			});
	}

	if (const auto& offeringsProblem = UMissionOfferingsUtil::QueryMissionOfferingsProblem(missionState)) {
		return FStartMissionStatus::fail(offeringsProblem.GetValue());
	}

	switch (missionRequest.requestType) {
	case EMissionRequestType::Continue:
		return FStartMissionStatus::success({ NSLOCTEXT("HyperMissionLabels", "hypermission_continue", "Continue") });
	case EMissionRequestType::Start:
	default:
		if (missionDef.isTutorial()) {
			return FStartMissionStatus::success({ NSLOCTEXT("", "missioninspector_starttutorial", "Start Tutorial") });
		}
		else {
			// Use the same string for missionDef.isHyperMission as "Start Ancient Hunt" doesn't fit in the buttons for Polish, etc
			return FStartMissionStatus::success({ NSLOCTEXT("", "missioninspector_startmission", "Start Mission") });
		}
	}
}

bool UMissionProgressComponent::QueryMissionRequestIsStartable(const FMissionRequest& missionRequest) const {
	return UStartMissionStatusUtil::IsStartable(QueryStartMissionStatus(missionRequest));
}

bool UMissionProgressComponent::IsUnlockedAndNotTutorial(EGameDifficulty difficulty, ELevelNames LevelName) const {
	return IsMissionUnlocked(difficulty, LevelName) && !IsMissionTutorial(LevelName);
}

const EndlessStruggleTier* UMissionProgressComponent::GetNextLockedEndlessStruggleTier(EGameDifficulty Difficulty) const {
	if (Difficulty >= difficultyquery::RequiredForEndlessStuggle) {
		return endlesstruggle::tier::getLowestLocked(this);
	}
	return nullptr;
}

const EndlessStruggleTier* UMissionProgressComponent::GetHighestUnlockedEndlessStruggleTier(EGameDifficulty Difficulty) const {
	if (Difficulty >= difficultyquery::RequiredForEndlessStuggle) {
		return endlesstruggle::tier::getHighestUnlocked(this);
	}
	return nullptr;
}

const FMissionState* UMissionProgressComponent::GetValidMissionState(ELevelNames levelName) const {
	if (const auto* missionState = GetCharacterSerializeComponent()->ReadMissionState(levelName)) {
		if (!missions::hajper::isAncientHuntDepleted(*missionState)) {
			return missionState;
		}
	}
	InternalConstForceClearMissionState(levelName);
	return nullptr;
}

bool UMissionProgressComponent::IsMissionOfInterest(EMissionInterest interestType, EGameDifficulty difficulty, ELevelNames levelName) const {
	// Completed trials should never be interesting.
	if (UMissionDefs::IsTrial(levelName) && HasCompletedCurrentTrialAtDifficulty(levelName, difficulty)) {
		return false;
	}

	if (missions::get(levelName).isNeverOfInterest()) {
		return false;
	}
	
	if (auto tier = GetNextLockedEndlessStruggleTier(difficulty)) {
		if (tier->IsMissionOfInterest(this, missions::get(levelName), interestType)) {
			return true;
		}
	}

	switch (interestType) {
	case EMissionInterest::NEW_MISSION:
		return IsMissionNew(difficulty, levelName);
	case EMissionInterest::NEW_REWARD:
		return DoesMissionHaveNewRewards(difficulty, levelName);
	case EMissionInterest::NEW_MERCHANT:
		return DoesMissionHaveNewCapturedMerchants(difficulty, levelName);
	case EMissionInterest::NEW_LOCATION:
		return DoesMissionHaveNewSecretLocations(difficulty, levelName);
	case EMissionInterest::NEW_ENDEREYE:
		return DoesMissionHaveNewEnderEye(difficulty, levelName);
	case EMissionInterest::PLAYABLE:
		return IsUnlockedAndNotTutorial(difficulty, levelName);
	case EMissionInterest::UNSET:
	default:
		return false;
	}
}

bool UMissionProgressComponent::HasCompletedCurrentTrialAtDifficulty(ELevelNames LevelName, EGameDifficulty Difficulty) const {
	 if (const auto trialId = GetWorld()->GetGameInstanceChecked<UDungeonsGameInstance>()->GetCurrentTrialId(LevelName)) {
		return GetCharacterSerializeComponent()->HasCompletedTrial(trialId.GetValue(), Difficulty);
	}
	return false;
}

TArray<EMissionInterest> UMissionProgressComponent::GetMissionFilteredInterestTypes(const TArray<EMissionInterest>& interestTypes, EGameDifficulty difficulty, ELevelNames levelName) const {
	return algo::copy_if(interestTypes, RETLAMBDA(IsMissionOfInterest(it, difficulty, levelName)));	
}

const TArray<EMissionInterest> UMissionProgressComponent::MissionInterestTypesToShowOnMissionMarker = {
	EMissionInterest::NEW_ENDEREYE,
	EMissionInterest::NEW_BOSS,
	EMissionInterest::NEW_LOCATION,
	EMissionInterest::NEW_REWARD,
	EMissionInterest::NEW_MERCHANT
};

TArray<EMissionInterest> UMissionProgressComponent::GetMissionMarkerInterestTypes(EGameDifficulty difficulty, ELevelNames levelName) const {
	return GetMissionFilteredInterestTypes(MissionInterestTypesToShowOnMissionMarker, difficulty, levelName);
}

FMissionRequest UMissionProgressComponent::PreviewMissionSelectionRequest(const FMissionSelection& selection) const {
	if (GetMissionCanContinue(selection.levelName)) {
		if (auto request = UMissionRequestUtil::InspectPlayerContinueMissionRequest(GetCharacterOwner(), selection.levelName)) {
			return request.GetValue();
		}
	} else {
		if (auto request = UMissionRequestUtil::InspectPlayerStartMissionRequest(GetCharacterOwner(), selection)) {
			return request.GetValue();
		}
	}
	return UMissionRequestUtil::CreateMissionRequest(GetCharacterOwner(), EMissionRequestType::Start, selection, 0, {});
}

TArray<ELevelNames> UMissionProgressComponent::GetMissionNamesOfInterestInDifficulty(EMissionInterest interestType, EGameDifficulty difficulty) const {
	return algo::copy_if(missionprogress::allSortedMissionNamesInDifficulty(difficulty),
		RETLAMBDA(IsMissionOfInterest(interestType, difficulty, it))
	);
}

int UMissionProgressComponent::CountMissionsOfInterestInDifficulty(EMissionInterest interestType, EGameDifficulty difficulty) const {
	return GetMissionNamesOfInterestInDifficulty(interestType, difficulty).Num();
}

FMissionInterestCarousel UMissionProgressComponent::GetBestMissionInterestCarouselInDifficulty(EGameDifficulty difficulty) const {
	auto CreateCarousel = [](EMissionInterest interestType, const TArray<ELevelNames>& missionNames) -> TOptional<FMissionInterestCarousel> {
		if (missionNames.Num() > 0) {
			return { {
				interestType,
				missionNames,
			} };
		}
		return {};
	};
	const TArray<EMissionInterest> CarouselPrio = {
		EMissionInterest::NEW_MISSION,
		EMissionInterest::NEW_MERCHANT,
		EMissionInterest::NEW_LOCATION,
		EMissionInterest::NEW_REWARD,
		EMissionInterest::NEW_BOSS,
		EMissionInterest::NEW_ENDEREYE,
		EMissionInterest::PLAYABLE,
	};

	for (auto interestType : CarouselPrio) {
		if (auto carousel = CreateCarousel(interestType, GetMissionNamesOfInterestInDifficulty(interestType, difficulty))) {
			return carousel.GetValue();
		}
	}

	return {};
}

bool UMissionProgressComponent::HasPreviouslyFoundItemType(FItemId ItemType) const {	
	const auto* save = GetOwner()->FindComponentByClass<UCharacterSerializeComponent>();
	return save && save->ReadItemsFound().Contains(ItemType);
}

//Query

bool UMissionProgressComponent::IsFulfilled(const FMissionDifficultyCompletion DifficultyCriteria) const
{
	switch (DifficultyCriteria.CompletionLogic) {
		case EMissionDifficultyCompletionLogic::DISABLED:
			return true;
		case EMissionDifficultyCompletionLogic::Any: 
			return GetNumCompletedMissionsAtDifficulty(DifficultyCriteria.DifficultyLevel) >= 1;
		case EMissionDifficultyCompletionLogic::UseCount:
			return GetNumCompletedMissionsAtDifficulty(DifficultyCriteria.DifficultyLevel) >= DifficultyCriteria.OptionalCount;
		case EMissionDifficultyCompletionLogic::UseLevelID:
			return GetHasCompletedMissionAtDifficulty(DifficultyCriteria.DifficultyLevel, DifficultyCriteria.OptionalLevelName);
		default:
		break;
	}

	return false;
}


EThreatLevel UMissionProgressComponent::GetMissionThreatLevelCompleted(ELevelNames LevelName) const {
	if (auto Completion = GetMissionProgress(LevelName)) {
		return Completion->completedThreatLevel;
	}
	return EThreatLevel::Invalid;
}

EGameDifficulty UMissionProgressComponent::GetMissionDifficultyCompleted(ELevelNames LevelName) const {
	if (auto Completion = GetMissionProgress(LevelName)) {
		return Completion->completedDifficulty;
	}
	return EGameDifficulty::Invalid;
}

bool UMissionProgressComponent::AreAllAvailableMissionDifficultiesCompleted(EGameDifficulty difficulty, ELevelNames LevelName) const {	
	for (int t = EThreatLevelFirstIndex; t < enum_cast(EThreatLevel::ENUM_COUNT); ++t) {		
		EThreatLevel threatLevel = static_cast<EThreatLevel>(t);
		if (IsMissionUnlocked(difficulty, LevelName) && !IsMissionDifficultyAndThreatLevelCompleted(difficulty, threatLevel, FEndlessStruggle(), LevelName)) {
			return false;
		}
	}	
	//At least one difficulty must be available
	return IsMissionUnlocked(difficulty, LevelName);
}

void UMissionProgressComponent::GetTotalEnderEyesCollected(int& outHeldEyes, int& outPortalEyes) {

	outHeldEyes = outPortalEyes = -1;

	if (auto characterSerialize = GetCharacterSerializeComponent()) {
		const auto& strongholdData = GetCharacterSerializeComponent()->GetStrongholdData();
		outPortalEyes = strongholdData.EyesPlacedInPortalCount();	

		UDungeonsGameInstance* gi = GetDungeonsGameInstance();

		if (ABasePlayerController* initialController = Cast<ABasePlayerController>(gi->GetUserManager()->GetInitialPlayerController())) {

			if (auto wallet = initialController->GetWalletComponent()) {
				outHeldEyes = wallet->Balance(game::item::type::EyeOfEnder.getId());
			}
		}
	}
}



bool UMissionProgressComponent::IsMissionDifficultyCompleted(EGameDifficulty Difficulty, ELevelNames LevelName) const
{
	if (UMissionDefs::IsHyperMission(LevelName)) {
		return false; //Hyper-missions are never completed
	} else if (UMissionDefs::IsTrial(LevelName)) {
		return HasCompletedCurrentTrialAtDifficulty(LevelName, Difficulty);
	} else if (auto Completion = GetMissionProgress(LevelName)) {
		return Completion->completedDifficulty >= Difficulty;
	}
	return false;
}


//State

const FMissionProgress* UMissionProgressComponent::GetMissionProgress(ELevelNames LevelName) const {
	auto mp = mCompletedMissions.FindByPredicate([&](const FMissionProgress &mc) { return mc.levelName == LevelName; });
	return mp;
}

FMissionProgress& UMissionProgressComponent::GetOrCreateEditableMissionProgress(ELevelNames LevelName) {
	if (FMissionProgress* mp = mCompletedMissions.FindByPredicate([&](const FMissionProgress& mc) { return mc.levelName == LevelName; })) {
		return *mp;
	}
	mCompletedMissions.Add({ LevelName });
	RefreshHighestCompletedMissionDifficulty();
	return mCompletedMissions.Last();
}



//Requirements
TArray<FMissionRequirement> UMissionProgressComponent::GetMissionUnlockRequirements(EGameDifficulty difficulty, ELevelNames LevelName) const
{
	TArray<FMissionRequirement> requirements;
	if (const auto mission = missions::getChecked(LevelName)) {

		if (auto type = mission->getRequiredEndGameContent()) {
			if (auto def = endgamecontent::defs().getChecked(type.GetValue())) {
				auto serialize = GetCharacterSerializeComponent();
				if (auto text = def->getUnlockRequirementText()) {
					requirements.Add({ def->isUnlocked(serialize), text.GetValue() });
				}
			}
		}

		if (auto requiredDifficulty = mission->getRequiredUnlockedDifficulty()) {
			auto minimal = requiredDifficulty.GetValue();
			requirements.Add({
				ShouldDifficultyBeUnlocked(minimal),
				UDifficultyUtil::GetUnlockRequirementText(minimal)
			});
		}

		for (auto requirement : mission->getRequiredMissions()) {
			requirements.Add({
				IsMissionCompleted(requirement),
				UMissionDefs::GetMissionAsRequirementText(requirement)
			});
		}

		if (auto unlockMission = mission->getSecretUnlockFoundInMission()) {
			requirements.Add({
				HasUnlockedBonusMission(LevelName),
				UMissionDefs::GetSecretUnlockInMissionAsRequirementText(unlockMission.GetValue())
			});
		}
	}
	return requirements;
}

bool UMissionProgressComponent::IsLevelUnlockedForAllLocalPlayers(const UObject * WorldContextObject, ELevelNames level){

	EGameDifficulty difficulty = EGameDifficulty::Difficulty_1;

	if (const auto* game = actorquery::getGame(WorldContextObject->GetWorld())) {
		difficulty = game->settings().difficulty.chosen();
	}

	return algo::all_of(InstanceTracker<APlayerCharacter>::GetList(WorldContextObject->GetWorld()), [difficulty, level](const APlayerCharacter* player) {
		return !player->IsLocallyControlled() || (player->GetMissionProgressComponent()->IsMissionUnlocked(difficulty, level));
	});
}





//Map UI state

FMapUIState UMissionProgressComponent::GetMapUIState() const
{
	return mMapUIState;
}


void UMissionProgressComponent::SetMapUIState(FMapUIState mapUiState)
{
	mMapUIState = mapUiState;
	SerializeSaveState();
}

TOptional<FMissionState> UMissionProgressComponent::GetMissionState(ELevelNames levelName) const {
	if (const auto* missionState = GetValidMissionState(levelName)) {
		return *missionState;
	} else {
		return {};
	}
}

bool UMissionProgressComponent::HasMissionState(ELevelNames levelName) const {
	return GetValidMissionState(levelName) != nullptr;
}

bool UMissionProgressComponent::ClearMissionState(ELevelNames levelName) {
	return InternalConstForceClearMissionState(levelName);
}

bool UMissionProgressComponent::InternalConstForceClearMissionState(ELevelNames levelName) const {
	if (GetCharacterSerializeComponent()->ForceClearMissionState(levelName)) {
		OnMissionStateChanged.Broadcast(levelName);
		return true;
	}
	return false;
}

void UMissionProgressComponent::SetMissionState(ELevelNames levelName, const FMissionState& missionState) {
	GetCharacterSerializeComponent()->SetMissionState(levelName, missionState);
	OnMissionStateChanged.Broadcast(levelName);
}

bool UMissionProgressComponent::GetMissionCanContinue(ELevelNames levelName) const {
	return missions::get(levelName).allowsContinue() && HasMissionState(levelName);
}

#if !UE_BUILD_SHIPPING
//Cheat
void UMissionProgressComponent::UnlockAllMissions(bool doUnlock)
{
	if (doUnlock != bUnlockAllMissions) {
		bUnlockAllMissions = doUnlock;
		OnUnlockAllChanged.Broadcast();
	}
}

void UMissionProgressComponent::UnlockAllRealms(bool doUnlock)
{
	if (doUnlock != bUnlockAllRealms) {
		bUnlockAllRealms = doUnlock;
		OnUnlockAllChanged.Broadcast();
	}
}

void UMissionProgressComponent::UnlockAllDLC(bool doUnlock)
{
	if (doUnlock != bUnlockAllDLC) {
		bUnlockAllDLC = doUnlock;
		OnUnlockAllChanged.Broadcast();
	}
}
#endif

const TSet<ELevelNames>& UMissionProgressComponent::GetUnlockedMissionsSet()
{
	if (!mUnlockedMissionsSetCache.IsSet()) {
		mUnlockedMissionsSetCache = TSet<ELevelNames>();
		for (const auto* missiondef : missions::getAllCurrentlyEnabled()) {
			if (IsMissionUnlocked(difficultyquery::First, missiondef->level())) {
				mUnlockedMissionsSetCache->Add(missiondef->level());
			}
		}
	}
	return mUnlockedMissionsSetCache.GetValue();
}

const TOptional<game::DifficultyStats>& UMissionProgressComponent::GetHighestCompletedDifficultyStats() {
	if (!HighestCompletedDifficultyStats) {
		auto serializer = GetCharacterSerializeComponent();
		if(serializer->HasProfile()){
			if(auto highest = serializer->GetHighestCompletedDifficulty()){
				HighestCompletedDifficultyStats = game::DifficultyStats(highest.GetValue());
			}
		}
	}
	return HighestCompletedDifficultyStats;
}

APlayerCharacter* UMissionProgressComponent::GetCharacterOwner() const {
	return Cast<APlayerCharacter>(GetOwner());
}

UCharacterSerializeComponent* UMissionProgressComponent::GetCharacterSerializeComponent() const {
	return GetCharacterOwner()->GetCharacterSerializeComponent();
}

void UMissionProgressComponent::SerializeSaveState() const {
	auto characterSerializer = GetCharacterSerializeComponent();

	UCharacterSaveData::MissionProgressMap SavedProgress;
	for (const FMissionProgress mc : mCompletedMissions) {
		MissionProgressSaveData mp;
		//save difficulty completed at			
		mp.completedDifficulty = mc.completedDifficulty != EGameDifficulty::Invalid ? mc.completedDifficulty : TOptional<EGameDifficulty>();
		mp.completedThreatLevel = mc.completedThreatLevel != EThreatLevel::Invalid ? mc.completedThreatLevel : TOptional<EThreatLevel>();
		mp.completedEndlessStruggle = mc.completedEndlessStruggle;
		
		//save level id			
		SavedProgress[mc.levelName] = mp;
	}
	characterSerializer->SetProgress(SavedProgress);

	DifficultyProgressSaveData SavedDifficultyProgress = characterSerializer->ReadDifficulties();
	SavedDifficultyProgress.unlocked = mDifficultyProgress.unlocked != EGameDifficulty::Invalid ? mDifficultyProgress.unlocked : TOptional<EGameDifficulty>();
	SavedDifficultyProgress.announced = mDifficultyProgress.announced != EGameDifficulty::Invalid ? mDifficultyProgress.announced : TOptional<EGameDifficulty>();
	SavedDifficultyProgress.selected = mDifficultyProgress.selected != EGameDifficulty::Invalid ? mDifficultyProgress.selected : TOptional<EGameDifficulty>();
	characterSerializer->SetDifficulties(SavedDifficultyProgress);

	ThreatLevelProgressSaveData SavedThreatLevelProgress = characterSerializer->ReadThreatLevels();
	SavedThreatLevelProgress.unlocked = mThreatLevelProgress.unlocked != EThreatLevel::Invalid ? mThreatLevelProgress.unlocked : TOptional<EThreatLevel>();
	SavedThreatLevelProgress.announced = mThreatLevelProgress.announced != EThreatLevel::Invalid ? mThreatLevelProgress.announced : TOptional<EThreatLevel>();
	characterSerializer->SetThreatLevelProgress(SavedThreatLevelProgress);

	EndGameContentProgressSaveData SavedAnnouncedUnlockedContent = characterSerializer->ReadEndGameContentProgress();
	for (auto item : mEndGameContentProgress.alreadyAnnouncedUnlockedContent)
		SavedAnnouncedUnlockedContent.announcedUnlockedContent.Add(item);
	characterSerializer->SetEndGameContentProgress(SavedAnnouncedUnlockedContent);

	auto& SavedMapUIState = characterSerializer->GetMapUIState();
	SavedMapUIState.selectedRealm = mMapUIState.selectedRealm != ERealmName::Invalid ? mMapUIState.selectedRealm : TOptional<ERealmName>();
	SavedMapUIState.selectedDifficulty = mMapUIState.selectedDifficulty != EGameDifficulty::Invalid ? mMapUIState.selectedDifficulty : TOptional<EGameDifficulty>();
	SavedMapUIState.selectedThreatLevel = mMapUIState.selectedThreatLevel != EThreatLevel::Invalid ? mMapUIState.selectedThreatLevel : TOptional<EThreatLevel>();
	SavedMapUIState.selectedMission = mMapUIState.selectedMission != ELevelNames::Invalid ? mMapUIState.selectedMission : TOptional<ELevelNames>();
	SavedMapUIState.panPosition = mMapUIState.panPosition;

	characterSerializer->SetUnlockedSecretMissions(mUnlockedSecretMissions);
}

void UMissionProgressComponent::DeserializeSaveState() {	
	auto characterSerializer = GetCharacterSerializeComponent();

	mCompletedMissions.Reset();
	mUnlockedMissionsSetCache.Reset();
	HighestCompletedDifficultyStats.Reset();

	for (auto&& pair : characterSerializer->GetProgress()) {
		FMissionProgress mc;

		auto LevelProgress = pair.second;
		mc.completedDifficulty = LevelProgress.completedDifficulty.Get(EGameDifficulty::Invalid);
		mc.completedThreatLevel = LevelProgress.completedThreatLevel.Get(EThreatLevel::Invalid);
		mc.completedEndlessStruggle = LevelProgress.completedEndlessStruggle;
		mc.levelName = pair.first;

		mCompletedMissions.Add(mc);
	}

	const auto& SavedDifficultyProgress = characterSerializer->ReadDifficulties();
	mDifficultyProgress = {
		SavedDifficultyProgress.unlocked.Get(EGameDifficulty::Invalid),
		SavedDifficultyProgress.announced.Get(EGameDifficulty::Invalid),
		SavedDifficultyProgress.selected.Get(EGameDifficulty::Invalid)
	};

	auto& SavedThreatLevelProgress = characterSerializer->ReadThreatLevels();
	mThreatLevelProgress = {
		SavedThreatLevelProgress.unlocked.Get(EThreatLevel::Invalid),
		SavedThreatLevelProgress.announced.Get(EThreatLevel::Invalid)
	};

	auto& EndGameContentProgress = characterSerializer->ReadEndGameContentProgress();
	mEndGameContentProgress = {
		EndGameContentProgress.announcedUnlockedContent
	};

	auto& SavedMapUIState = characterSerializer->GetMapUIState();
	mMapUIState = {
		SavedMapUIState.selectedRealm.Get(realms::Default),
		SavedMapUIState.selectedDifficulty.Get(difficultyquery::Default),
		SavedMapUIState.selectedMission.Get(missions::Default),
		SavedMapUIState.selectedThreatLevel.Get(threatquery::Default),
		SavedMapUIState.panPosition.Get(FVector2D(0.0f, 0.0f))
	};

	mUnlockedSecretMissions = characterSerializer->GetUnlockedSecretMissions();
	RefreshHighestCompletedMissionDifficulty();
	OnMissionsCompletedChanged.Broadcast();
	OnMissionsProgressLoaded.Broadcast();
}

const int UMissionProgressComponent::GetCurrentMissionRecommendedItemPower() {
	FMissionDifficulty difficulty = { mMapUIState.selectedMission, mMapUIState.selectedDifficulty, mMapUIState.selectedThreatLevel };
	return UMissionDefs::GetRecommendedTotalGearItemPower(difficulty);
}
