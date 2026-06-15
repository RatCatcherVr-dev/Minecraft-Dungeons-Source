#include "Dungeons.h"
#include "game/actor/character/player/PlayerCharacterSaveSlot.h"
#include "MediaSource.h"
//F:\projects\UnrealEngine4\Engine\Source\Runtime\MediaAssets\Public\MediaSource.h
#include "EquipmentComponent.h"
#include "util/Algo.hpp"
#include "CharacterSerializeComponent.h"
#include "game/reward/RewardData.h"


UCharacterSerializeComponent::UCharacterSerializeComponent() {
	PrimaryComponentTick.bCanEverTick = false;	
	bReplicates = false;

	if(!IsTemplate())
	{	
		mDefaultSaveData = NewObject<UCharacterSaveData>(this, TEXT("DefaultSaveData"));
		mSaveData = mDefaultSaveData; //this is relied on too much and should never be null
	}
}

void UCharacterSerializeComponent::AssignCharacter(UCharacterSaveData* character) {
	if (character)
	{
		mSaveData = character;
	}
	else
	{
		mSaveData = mDefaultSaveData; //this is relied on too much and should never be null
	}
	OnCharacterAssigned();	
}

bool UCharacterSerializeComponent::HasProfile() const {
	return mSaveData != mDefaultSaveData;
}

const FString& UCharacterSerializeComponent::ReadName() const {
	return mSaveData->mRecordedData.name;
}

void UCharacterSerializeComponent::SetName(FString name) {
	mSaveData->mRecordedData.name = name;
	IncrementDirtyFlag(100);
}

const FName& UCharacterSerializeComponent::ReadSkin() const {
	return mSaveData->mRecordedData.skin;
}

const FGuid UCharacterSerializeComponent::GetCloudPlayerId()
{
	return mSaveData->GetPlayerId();
}

void UCharacterSerializeComponent::SetSkin(FName skinId) {
	mSaveData->mRecordedData.skin = skinId;
	IncrementDirtyFlag(100);
}

uint32 UCharacterSerializeComponent::ReadCurrencyFor(const FItemId& inType) const {
	return mSaveData->getCurrencyFor(inType);
}


void UCharacterSerializeComponent::SetCurrencyFor(const FItemId& inType, uint32 inValue) {
	const auto it = std::find_if(mSaveData->mRecordedData.currency.begin(), mSaveData->mRecordedData.currency.end(), [inType](const CurrencySaveData& v) { return v.type == inType; });
	if (it != mSaveData->mRecordedData.currency.end()) {
		IncrementDirtyFlag(inValue - it->count);
		it->count = inValue;
	}
	else {
		mSaveData->mRecordedData.currency.emplace_back(CurrencySaveData{ inType, inValue });
		IncrementDirtyFlag(inValue);
	}
}

TArray<const FItemId*> UCharacterSerializeComponent::ReadOwnedCurrencyTypes() const {
	return mSaveData->getOwnedCurrencyTypes();
}

int32 UCharacterSerializeComponent::ReadMobKillsForType(EntityType inType) const {
	const auto mobType = mSaveData->mRecordedData.mobKills.find(inType);
	if (mobType != mSaveData->mRecordedData.mobKills.end()) {
		return mobType->second;
	}

	return 0;
}

void UCharacterSerializeComponent::AddMobKillForType(EntityType inType) {
	auto mobType = mSaveData->mRecordedData.mobKills.find(inType);
	if (mobType != mSaveData->mRecordedData.mobKills.end()) {
		mobType->second += 1;
	}
	else {
		mSaveData->mRecordedData.mobKills[inType] = 1;
	}

	IncrementDirtyFlag(1);
}

void UCharacterSerializeComponent::ChestUnlocked(int32 chestId) {
	auto& chests = mSaveData->mRecordedData.unlockedLobbyChests;

	ChestSaveData& chestProgress = chests[chestId];
	chestProgress.unlockedTimes++;

	IncrementDirtyFlag(10);
}

int32 UCharacterSerializeComponent::GetChestUnlockedTimes(int32 chestId) const {
	auto it = mSaveData->mRecordedData.unlockedLobbyChests.find(chestId);
	if (it != mSaveData->mRecordedData.unlockedLobbyChests.end()) {
		return it->second.unlockedTimes;
	}
	return 0;
}

UCharacterSaveData::MissionProgressMap& UCharacterSerializeComponent::GetProgress() {
	IncrementDirtyFlag(10);
	return mSaveData->mRecordedData.progress;
}

const UCharacterSaveData::MissionProgressMap& UCharacterSerializeComponent::ReadProgress() const {
	return mSaveData->mRecordedData.progress;
}

void UCharacterSerializeComponent::SetProgress(const UCharacterSaveData::MissionProgressMap& inProgress) {
	mSaveData->mRecordedData.progress = inProgress;
	IncrementDirtyFlag(10);
}

const StrongholdProgressSaveData& UCharacterSerializeComponent::GetStrongholdData() const {
	return mSaveData->mRecordedData.strongholdProgress;
}

void UCharacterSerializeComponent::SetStrongholdData(StrongholdProgressSaveData strongholdData) {
	mSaveData->mRecordedData.strongholdProgress = strongholdData;
	IncrementDirtyFlag(10);
}

void UCharacterSerializeComponent::SetStrongholdUsedPortal() {
	mSaveData->mRecordedData.strongholdProgress.usedPortal = true;
}

bool UCharacterSerializeComponent::HasUsedPortal() const {
	return mSaveData->mRecordedData.strongholdProgress.usedPortal;
}

bool UCharacterSerializeComponent::HasVideoBeenPlayed(const UMediaSource& mediaSource) const {
	return mSaveData->mRecordedData.videosPlayed.Contains(mediaSource.GetUrl());
}

void UCharacterSerializeComponent::SetVideoWasPlayed(const UMediaSource& mediaSource) {
	mSaveData->mRecordedData.videosPlayed.Add(mediaSource.GetUrl());
}

TOptional<game::FDifficulty> UCharacterSerializeComponent::GetHighestCompletedDifficulty() const {
	const TArray<game::FDifficulty> validDifficulties = algo::copy_if_map_tarray(ReadProgress(),
		RETLAMBDA(it.second.completedDifficulty && it.second.completedThreatLevel),
		RETLAMBDA(game::FDifficulty(it.second.completedDifficulty.GetValue(), it.second.completedThreatLevel.GetValue(), EExtraChallenge::NoExtraChallenge, it.second.completedEndlessStruggle)));

	return algo::max_element_by(validDifficulties, RETLAMBDA(it.combined()));
}

bool UCharacterSerializeComponent::IsDifficultyCompleted(EGameDifficulty difficulty, EThreatLevel threat) const {
	if (auto highest = GetHighestCompletedDifficulty()) {
		FMissionDifficulty compare{ ELevelNames::Invalid, difficulty, threat, FEndlessStruggle() };
		return highest.GetValue() >= compare;
	}
	return false;
}

bool UCharacterSerializeComponent::IsThreatLevelCompleted(EThreatLevel ThreatLevel) const {
	if (auto highest = GetHighestCompletedDifficulty()) {
		FMissionDifficulty compare{ ELevelNames::Invalid, EGameDifficulty::Difficulty_1, ThreatLevel, FEndlessStruggle() };
		return highest.GetValue() >= compare;
	}
	return false;
}

bool UCharacterSerializeComponent::IsLevelCompleted(FMissionDifficulty missionDifficulty) const
{
	const TArray<game::FDifficulty> validDifficulties = algo::copy_if_map_tarray(ReadProgress(),
		RETLAMBDA(it.first == missionDifficulty.mission && it.second.completedDifficulty.GetValue() >= missionDifficulty.difficulty && it.second.completedThreatLevel.GetValue() >= missionDifficulty.threatLevel),
		RETLAMBDA(game::FDifficulty(it.second.completedDifficulty.GetValue(), it.second.completedThreatLevel.GetValue(), EExtraChallenge::NoExtraChallenge, it.second.completedEndlessStruggle)));

	return validDifficulties.Num() > 0;
}

const DifficultyProgressSaveData& UCharacterSerializeComponent::ReadDifficulties() const {
	return mSaveData->mRecordedData.difficulties;
}

void UCharacterSerializeComponent::SetDifficulties(DifficultyProgressSaveData difficulties) {
	mSaveData->mRecordedData.difficulties = difficulties;
	IncrementDirtyFlag(1);
}

const ThreatLevelProgressSaveData& UCharacterSerializeComponent::ReadThreatLevels() const {
	return mSaveData->mRecordedData.threatLevels;
}

void UCharacterSerializeComponent::SetThreatLevelProgress(const ThreatLevelProgressSaveData& threatLevel) {
	mSaveData->mRecordedData.threatLevels = threatLevel;
	IncrementDirtyFlag(1);
}

const EndGameContentProgressSaveData& UCharacterSerializeComponent::ReadEndGameContentProgress() const {
	return mSaveData->mRecordedData.endGameContentProgress;
}

void UCharacterSerializeComponent::SetEndGameContentProgress(const EndGameContentProgressSaveData& endGameContentProgress) {
	mSaveData->mRecordedData.endGameContentProgress = endGameContentProgress;
	IncrementDirtyFlag(1);
}

const TSet<ELevelNames>& UCharacterSerializeComponent::GetUnlockedSecretMissions() const {
	return mSaveData->mRecordedData.unlockedSecretMissions;
}

void UCharacterSerializeComponent::SetUnlockedSecretMissions(const TSet<ELevelNames>& unlockedSecretMissions) {
	mSaveData->mRecordedData.unlockedSecretMissions = unlockedSecretMissions;
	IncrementDirtyFlag(1);
}

int UCharacterSerializeComponent::GetProgressStat(EProgressStat stat) const {
	return mSaveData->mRecordedData.progressStatCounters.Find(stat) ? mSaveData->mRecordedData.progressStatCounters[stat] : 0;
}

void UCharacterSerializeComponent::IncrementProgressStat(EProgressStat stat, int count /*=1*/) {
	mSaveData->mRecordedData.progressStatCounters.Add(stat, GetProgressStat(stat) + count);
}

const TArray<CosmeticsSaveData>& UCharacterSerializeComponent::GetCosmetics() const {
	return mSaveData->mRecordedData.cosmetics;
}

void UCharacterSerializeComponent::SetCosmetics(const TArray<CosmeticsSaveData>& cosmetics) {
	mSaveData->mRecordedData.cosmetics = cosmetics;
	IncrementDirtyFlag(1);
}

void UCharacterSerializeComponent::SetCosmeticEquipped(FName cosmeticId) {
	mSaveData->mRecordedData.cosmeticsEverEquipped.Add(cosmeticId);
}

bool UCharacterSerializeComponent::HasEverEquippedCosmetic(FName cosmeticId) const {
	return mSaveData->mRecordedData.cosmeticsEverEquipped.Contains(cosmeticId);
}

const std::unordered_map<std::string, int32>& UCharacterSerializeComponent::GetFinishedObjectiveTags() const {
	return mSaveData->mRecordedData.finishedObjectiveTags;
}

void UCharacterSerializeComponent::FinishedObjectiveTag(const std::string& tagName) {
	IncrementObjectiveTag(tagName, 1);
}

void UCharacterSerializeComponent::IncrementObjectiveTag(const std::string& tagName, int count /*= 1*/) {
	mSaveData->mRecordedData.finishedObjectiveTags[tagName] += count;
	IncrementDirtyFlag(1);
}

int UCharacterSerializeComponent::GetFinishedObjectiveTagCount(const std::string& tagName) const {
	auto tags = GetFinishedObjectiveTags();
	auto foundIter = tags.find(tagName);
	if (foundIter != tags.end()) {
		return (*foundIter).second;
	}
	return 0;
}

MapUIState& UCharacterSerializeComponent::GetMapUIState() {
	IncrementDirtyFlag(1);
	return mSaveData->mRecordedData.mapUIState;
}

std::vector<ItemSaveData>& UCharacterSerializeComponent::GetItems() {
	IncrementDirtyFlag(1);
	return mSaveData->mRecordedData.items;
}

void UCharacterSerializeComponent::GetItemsCopy(std::vector<ItemSaveData>& Dest)const {
	Dest = mSaveData->mRecordedData.items;
}

const TSet<FItemId>& UCharacterSerializeComponent::ReadItemsFound() const {
	return mSaveData->mRecordedData.itemsFound;
}

TSet<FItemId>& UCharacterSerializeComponent::GetItemsFound() {
	IncrementDirtyFlag(1);
	return mSaveData->mRecordedData.itemsFound;
}

const TSet<FItemId>& UCharacterSerializeComponent::ReadCurrenciesFound() const {
	return mSaveData->mRecordedData.currenciesFound;
}

TSet<FItemId>& UCharacterSerializeComponent::GetCurrenciesFound() {
	IncrementDirtyFlag(1);
	return mSaveData->mRecordedData.currenciesFound;
}

int32 UCharacterSerializeComponent::ReadXP() const {
	return mSaveData->mRecordedData.xp;
}

void UCharacterSerializeComponent::SetXP(int32 inXp) {
	mSaveData->mRecordedData.xp = inXp;
	IncrementDirtyFlag(1);
}

const FGuid& UCharacterSerializeComponent::ReadPlayerGuid() const {
	return mSaveData->mRecordedData.playerId;
}

std::vector<UIHintProgressSaveData>& UCharacterSerializeComponent::GetExpiredUiHints() {
	IncrementDirtyFlag(1);
	return mSaveData->mRecordedData.uiHintsExpired;
}

// reward items: 
TOptional<FRewardData> UCharacterSerializeComponent::ClaimPendingRewardItem() {
	IncrementDirtyFlag(1);
	return mSaveData->claimNextPendingRewardItem();
}

const FRewardData& UCharacterSerializeComponent::PeekPendingReward() const {
	return mSaveData->getNextPendingRewardItem();
}

void UCharacterSerializeComponent::AddPendingRewardItem(const FRewardData& item) {
	mSaveData->addPendingRewardItem(item);
	IncrementDirtyFlag(100);
}

const TArray<FString>& UCharacterSerializeComponent::GetUnlockKeys() const {
	return mSaveData->mRecordedData.progressionKeys;
}

bool UCharacterSerializeComponent::HasPendingRewardItem() const {
	return mSaveData && mSaveData->hasPendingRewardItem();
}

int UCharacterSerializeComponent::NumPendingRewards() const
{
	return mSaveData ? mSaveData->numPendingRewards() : 0;
}

bool UCharacterSerializeComponent::AddUnlockKey(const FString& key) {
	if (algo::add_unique_if(mSaveData->mRecordedData.progressionKeys, key, RETLAMBDA(key.Equals(it, ESearchCase::IgnoreCase)))) {
		IncrementDirtyFlag(100);
		return true;
	}
	return false;
}

bool UCharacterSerializeComponent::RemoveUnlockKey(const FString& key) {
	int numRemovedElements = mSaveData->mRecordedData.progressionKeys.Remove(key);
	if (numRemovedElements > 0) {
		IncrementDirtyFlag(100);
	}
	return numRemovedElements > 0;
}

bool UCharacterSerializeComponent::HasUnlockKey(const FString& key) const {
	return mSaveData->mRecordedData.progressionKeys.Contains(key);
}

void UCharacterSerializeComponent::SetLegendaryStatus(ELegendaryStatus newStatus) {
	mSaveData->mRecordedData.legendaryStatus = newStatus;
	IncrementDirtyFlag(100);
}

ELegendaryStatus UCharacterSerializeComponent::GetLegendaryStatus() {
	return mSaveData->GetLegendaryStatus();
}

void UCharacterSerializeComponent::AddTrialCompleted(const FString& trialId, EGameDifficulty difficulty) {
	return mSaveData->addTrialCompleted(trialId, difficulty);
}

bool UCharacterSerializeComponent::HasCompletedTrial(const FString& trialId, EGameDifficulty difficulty) const {
	return mSaveData->hasCompletedTrial(trialId, difficulty);
}


//Hyper-Mission
bool UCharacterSerializeComponent::HasMissionState(ELevelNames mission) const {
	return mSaveData->mRecordedData.missionStates.Contains(mission);
}

bool UCharacterSerializeComponent::ClearMissionState(ELevelNames mission, const FString& guid) {
	if (auto* missionState = mSaveData->mRecordedData.missionStates.Find(mission)) {
		if (missionState->guid == guid) {
			mSaveData->mRecordedData.missionStates.Remove(mission);
			IncrementDirtyFlag(100);
			return true;
		}
	}
	return false;
}

bool UCharacterSerializeComponent::ForceClearMissionState(ELevelNames mission) {
	if (mSaveData->mRecordedData.missionStates.Remove(mission)) {
		IncrementDirtyFlag(100);
		return true;
	}
	return false;
}

void UCharacterSerializeComponent::SetMissionState(ELevelNames mission, const FMissionState& state) {
	mSaveData->mRecordedData.missionStates.Add(mission, state);
	IncrementDirtyFlag(100);
}

const FMissionState* UCharacterSerializeComponent::ReadMissionState(ELevelNames mission) const {
	return mSaveData->mRecordedData.missionStates.Find(mission);
}

FMissionState* UCharacterSerializeComponent::EditMissionState(ELevelNames mission) {
	if (auto* missionState = mSaveData->mRecordedData.missionStates.Find(mission)) {
		IncrementDirtyFlag(10);
		return missionState;
	}
	return nullptr;
}

FMissionState* UCharacterSerializeComponent::EditMissionState(ELevelNames mission, const FString& guid) {
	if (auto* missionState = mSaveData->mRecordedData.missionStates.Find(mission)) {
		if (missionState->guid == guid) {
			IncrementDirtyFlag(10);
			return missionState;
		}
	}
	return nullptr;
}

void UCharacterSerializeComponent::ClearTrial_DEBUG(const FString& trialId, EGameDifficulty difficulty) {
	mSaveData->clearTrial(trialId, difficulty);
}

void UCharacterSerializeComponent::ClearTrials_DEBUG() {
	mSaveData->clearTrials();
}

bool UCharacterSerializeComponent::HasMechantSaveData(const FName& merchantId) {
	return mSaveData->mRecordedData.merchantData.Contains(merchantId);
}

const FMerchantSaveData& UCharacterSerializeComponent::ReadMechantSaveData(const FName& merchantId) {
	return mSaveData->mRecordedData.merchantData[merchantId];
}

void UCharacterSerializeComponent::CreateMechantSaveData(const FName& merchantId) {
	IncrementDirtyFlag(10);
	mSaveData->mRecordedData.merchantData.Add(merchantId, {});
}

FMerchantSaveData& UCharacterSerializeComponent::EditMechantSaveData(const FName& merchantId) {
	IncrementDirtyFlag(10);
	return mSaveData->mRecordedData.merchantData[merchantId];
}

bool UCharacterSerializeComponent::HasUnlockedMerchant(const TSubclassOf<UMerchantDef> merchant) const {
	return HasUnlockKey(merchant->GetDefaultObject<UMerchantDef>()->GetUnlockProgressKey());
}

bool UCharacterSerializeComponent::UnlockMerchant(const TSubclassOf<UMerchantDef> merchant) {
	return AddUnlockKey(merchant->GetDefaultObject<UMerchantDef>()->GetUnlockProgressKey());
}

UCharacterSaveData* UCharacterSerializeComponent::GetSaveData() const {
	return mSaveData;
}

const EndGameContentProgressSaveData& UCharacterSerializeComponent::ReadAnnouncedEndGameContent() const {
	return mSaveData->mRecordedData.endGameContentProgress;
}

void UCharacterSerializeComponent::EndPlay(EEndPlayReason::Type Reason) {
	Super::EndPlay(Reason);
}

void UCharacterSerializeComponent::BeginPlay() {
	Super::BeginPlay();
}

