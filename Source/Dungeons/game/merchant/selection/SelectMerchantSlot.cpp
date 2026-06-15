#include "Dungeons.h"
#include "game/merchant/slot/MerchantSlotBase.h"
#include "game/merchant/type/MerchantBase.h"
#include "SelectMerchantSlot.h"

UMerchantSlotBase* USelectMerchantSlot::GetMerchantSlot() const {
	return mSelectedSlot;
}

void USelectMerchantSlot::EnsureSelection() {
	if (GetMerchantSlot() == nullptr) {
		auto slots = GetSelectableMerchantSlots();
		if (slots.IsValidIndex(0)) {
			SelectMerchantSlot(slots[0]);
		}
	}
}

void USelectMerchantSlot::ClearSelection() {
	if (GetMerchantSlot()) {
		SelectMerchantSlot(nullptr);
	}
}

TArray<TSubclassOf<UMerchantTransactionBase>> USelectMerchantSlot::GetAvailableTransactions() const {
	if (auto slot = GetMerchantSlot()) {
		return slot->GetAvailableTransactions();
	}
	return Super::GetAvailableTransactions();
}

TArray<UMerchantSlotBase*> USelectMerchantSlot::GetSelectableMerchantSlots() const {
	return GetContext().GetMerchant().GetSlots();
}

void USelectMerchantSlot::SelectMerchantSlot(UMerchantSlotBase* merchantSlot) {
	mSelectedSlot = merchantSlot;
	SelectionChanged();
}

bool USelectMerchantSlot::CanSelectAny() const {
	return GetSelectableMerchantSlots().Num() > 0;
}

bool USelectMerchantSlot::HasSelectedAny() const {
	return GetMerchantSlot() != nullptr;
}
