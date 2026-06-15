#include "Dungeons.h"
#include "game/merchant/type/MerchantBase.h"
#include "game/merchant/currency/MerchantCurrencyComponent.h"
#include "MerchantSlotBase.h"


void UMerchantSlotBase::WasChanged() const {
	OnMerchantSlotChanged.Broadcast();
	OnMerchantSlotChangedInternal.Broadcast();
}

void UMerchantSlotBase::SetUnlockQuest(UMerchantQuestBase* quest) {
	mUnlockQuest = quest;
	WasChanged();
}

bool UMerchantSlotBase::HasCompletedUnlockQuest() const {
	if (mUnlockQuest && mUnlockQuest->HasSaveData()) {
		return mUnlockQuest->IsCompleted();
	}
	return false;
}



bool UMerchantSlotBase::ShouldShowObjectiveMarker() const {
	return false;
}

UMerchantQuestBase* UMerchantSlotBase::GetActiveUnlockQuest() const {
	if (mUnlockQuest && !mUnlockQuest->IsCompleted()) {
		return mUnlockQuest;
	}
	return nullptr;
}

bool UMerchantSlotBase::IsUnlocked() const {
	if (mUnlockQuest) {
		return mUnlockQuest->IsCompleted();
	}
	return true;
}

bool UMerchantSlotBase::HasPrice() const {
	return GetOptionalPrice().IsSet();
}

FMerchantDisplayPrice UMerchantSlotBase::GetPrice() const {
	return GetSession().GetCurrencyComponent()->AsMerchantDisplayPrice(GetOptionalPrice());
}

TSet<TSubclassOf<UMerchantSelectionBase>> UMerchantSlotBase::GetRequiredSelectionClasses() const {
	TSet<TSubclassOf<UMerchantSelectionBase>> selectionClasses;
	PopulateRequiredSelectionClasses(selectionClasses);
	return selectionClasses;
}

void UMerchantSlotBase::EnsureSaveData() {
	auto& saveData = GetContext().EditSaveData();
	if (!saveData.HasSlot(GetFName())) {
		saveData.CreateSlot(GetFName());
	}
	if (mUnlockQuest) {
		mUnlockQuest->EnsureSaveData();
	}
}

const FMerchantSlotSaveData& UMerchantSlotBase::ReadSaveData() const {
	return GetContext().ReadSaveData().ReadSlot(GetFName());
}

FMerchantSlotSaveData& UMerchantSlotBase::EditSaveData() const {
	return GetContext().EditSaveData().EditSlot(GetFName());
}