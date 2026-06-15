#include "MerchantSaveData.h"

bool FMerchantSaveData::HasInteracted() const {
	return mEverInteracted;
}

void FMerchantSaveData::SetInteracted(bool interacted) {
	mEverInteracted = interacted;
}

bool FMerchantSaveData::HasSlot(const FName& id) const {
	return mSlots.Contains(id);
}

void FMerchantSaveData::CreateSlot(const FName& id) {
	check(!HasSlot(id) && "slot data already created");
	mSlots.Add(id, {});
}

const FMerchantSlotSaveData& FMerchantSaveData::ReadSlot(const FName& id) const {
	return *mSlots.Find(id);
}

FMerchantSlotSaveData& FMerchantSaveData::EditSlot(const FName& id) {
	return *mSlots.Find(id);
}

bool FMerchantSaveData::HasQuest(const FName& id) const {
	return mQuests.Contains(id);
}

FMerchantQuestSaveData& FMerchantSaveData::CreateQuest(const FName& id) {
	check(!HasQuest(id) && "quest data already created");
	return mQuests.Add(id, {});
}

const FMerchantQuestSaveData& FMerchantSaveData::ReadQuest(const FName& id) const {
	return *mQuests.Find(id);
}

FMerchantQuestSaveData& FMerchantSaveData::EditQuest(const FName& id) {
	return *mQuests.Find(id);
}



const FMerchantPricingSaveData& FMerchantSaveData::ReadPricing() const {
	return mPricing;
}

FMerchantPricingSaveData& FMerchantSaveData::EditPricing(){
	return mPricing;
}


bool FCountQuestState::IsCompleted(int count) const {
	return count >= mTargetCount;
}

float FCountQuestState::GetFraction(int count) const {
	auto deltaCount = GetDeltaCountClamped(count);
	auto deltaTargetCount = GetDeltaTargetCount();
	if (deltaTargetCount > 0) {
		return static_cast<float>(deltaCount) / static_cast<float>(deltaTargetCount);
	}
	return 1.0f;
}

int FCountQuestState::GetDeltaCountClamped(int count) const {
	return FMath::Clamp(count - mStartedAtCount, 0, GetDeltaTargetCount());
}

int FCountQuestState::GetDeltaCount(int count) const {
	return count - mStartedAtCount;
}

int FCountQuestState::GetDeltaTargetCount() const {
	return mTargetCount - mStartedAtCount;
}




void FMerchantPricingSaveData::IncrementRestocked() {
	mTimesRestocked++;
}

int FMerchantPricingSaveData::GetTimesRestocked() const {
	return mTimesRestocked;
}

void FMerchantPricingSaveData::ResetTimesRestocked() {
	mTimesRestocked = 0;
}




TOptional<EProgressStat> FDynamicQuestState::getSelectedProgressStat() const {
	return mSelectedProgressStat;
}

void FDynamicQuestState::setSelectedProgressStat(EProgressStat progressStat) {
	mSelectedProgressStat = progressStat;
}

bool FMerchantQuestSaveData::HasDynamicState() const {
	return mDynamicQuestState.IsSet();
}

const TOptional<FDynamicQuestState>& FMerchantQuestSaveData::ReadDynamicState() const {
	return mDynamicQuestState;
}

FDynamicQuestState& FMerchantQuestSaveData::CreateDynamicState() {
	check(!HasDynamicState() && "quest dynamic data already created");
	mDynamicQuestState = { FDynamicQuestState{} };
	return mDynamicQuestState.GetValue();
}

FDynamicQuestState& FMerchantQuestSaveData::EditDynamicState() {
	return mDynamicQuestState.GetValue();
}
