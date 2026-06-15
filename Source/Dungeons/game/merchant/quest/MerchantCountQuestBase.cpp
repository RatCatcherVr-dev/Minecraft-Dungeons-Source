#include "Dungeons.h"
#include "game/progress/ProgressStat.h"
#include "MerchantCountQuestBase.h"
#include "game/util/ValueFormat.h"

void UMerchantCountQuestBase::InitSaveData(FMerchantQuestSaveData& saveData) const {
	auto count = GetProgressCount();
	saveData.mCountQuestState = {
		count + mCountToComplete,
		count,
	};
}

TOptional<FCountQuestState> UMerchantCountQuestBase::ReadCountQuestState() const {
	return ReadSaveData().mCountQuestState;
}

FText UMerchantCountQuestBase::GetCountText() const {
	return valueformat::asCounter(GetDisplayCount(), GetDisplayTargetCount());
}

bool UMerchantCountQuestBase::IsCompleted() const {
	if (auto questState = ReadCountQuestState()) {
		return questState.GetValue().IsCompleted(GetProgressCount());
	}
	return false;
}

float UMerchantCountQuestBase::GetProgressFraction() const {
	if (auto questState = ReadCountQuestState()) {
		return questState.GetValue().GetFraction(GetProgressCount());
	}
	return 0.0f;	
}

int UMerchantCountQuestBase::GetCompletedCount() const {
	if (auto questState = ReadCountQuestState()) {
		auto targetDeltaCount = ReadCountQuestState().GetValue().GetDeltaTargetCount();
		if (targetDeltaCount > 0){
			return ReadCountQuestState().GetValue().GetDeltaCount(GetProgressCount()) / targetDeltaCount;
		}
	}
	return 0;
}

int UMerchantCountQuestBase::GetDisplayCount() const {
	if (auto questState = ReadCountQuestState()) {
		return ReadCountQuestState().GetValue().GetDeltaCountClamped(GetProgressCount());
	}
	return 0;
}

int UMerchantCountQuestBase::GetDisplayTargetCount() const {
	if (auto questState = ReadCountQuestState()) {
		return questState.GetValue().GetDeltaTargetCount();
	}
	return mCountToComplete;
}