#include "Dungeons.h"
#include "game/merchant/type/MerchantBase.h"
#include "MerchantQuestBase.h"

bool UMerchantQuestBase::IsCompleted() const {	
	return false;	
}

int UMerchantQuestBase::GetCompletedCount() const {
	return IsCompleted() ? 1 : 0;
}

float UMerchantQuestBase::GetProgressFraction() const {
	return 0.0f;
}

FText UMerchantQuestBase::GetCountText() const {
	return FText::GetEmpty();
}

FText UMerchantQuestBase::GetProgressText() const {
	return FText::GetEmpty();
}

FText UMerchantQuestBase::GetExplainerText() const {
	return FText::GetEmpty();
}

bool UMerchantQuestBase::HasSaveData() const {
	return GetContext().ReadSaveData().HasQuest(GetFName());
}

const FMerchantQuestSaveData& UMerchantQuestBase::ReadSaveData() const {
	return GetContext().ReadSaveData().ReadQuest(GetFName());
}

FMerchantQuestSaveData& UMerchantQuestBase::EditSaveData() const {
	return GetContext().EditSaveData().EditQuest(GetFName());
}

bool UMerchantQuestBase::EnsureSaveData() const {
	auto& saveData = GetContext().EditSaveData();
	if (!saveData.HasQuest(GetFName())) {		
		InitSaveData(saveData.CreateQuest(GetFName()));
		return true;
	}
	return false;
}

void UMerchantQuestBase::ResetProgress() const {
	InitSaveData(EditSaveData());
}


const TOptional<FDynamicQuestState>& UMerchantQuestBase::ReadDynamicQuestState() const {
	return ReadSaveData().ReadDynamicState();
}

FDynamicQuestState& UMerchantQuestBase::EditOrCreateDynamicQuestState() const {
	auto& data = EditSaveData();
	if (!data.HasDynamicState()) {
		return data.CreateDynamicState();
	}
	return data.EditDynamicState();
}
