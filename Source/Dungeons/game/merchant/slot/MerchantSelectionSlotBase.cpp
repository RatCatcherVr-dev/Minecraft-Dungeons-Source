#include "Dungeons.h"
#include "game/merchant/type/MerchantBase.h"
#include "game/merchant/selection/MerchantSelectionBase.h"
#include "MerchantSelectionSlotBase.h"


void UMerchantSelectionSlotBase::OnSelectionChanged(const UMerchantSelectionBase& selection) const {
	OnMerchantSelectionSlotChanged.Broadcast(&selection);
}

void UMerchantSelectionSlotBase::PopulateRequiredSelectionClasses(TSet<TSubclassOf<UMerchantSelectionBase>>& selections) const {
	if (mSelectionClass) {
		selections.Add(mSelectionClass);
	}
}

UMerchantSelectionBase* UMerchantSelectionSlotBase::GetSelection() const {
	return GetMerchant()->GetSelectionByClass(mSelectionClass);
}

const TSubclassOf<UMerchantSelectionBase>& UMerchantSelectionSlotBase::GetSelectionClass() const {
	return mSelectionClass;
}

