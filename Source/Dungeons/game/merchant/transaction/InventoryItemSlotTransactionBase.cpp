#include "Dungeons.h"
#include "game/merchant/selection/SelectInventorySlotItem.h"
#include "game/merchant/type/MerchantBase.h"
#include "InventoryItemSlotTransactionBase.h"

void UInventoryItemSlotTransactionBase::PopulateRequiredSelectionClasses(TSet<TSubclassOf<UMerchantSelectionBase>>& selections) const {
	Super::PopulateRequiredSelectionClasses(selections);
	if(auto selectionClass = GetInventorySlotSelectionClass()){
		selections.Add(selectionClass);
	}
}

TSubclassOf<USelectInventorySlotItem> UInventoryItemSlotTransactionBase::GetInventorySlotSelectionClass() const {
	return USelectInventorySlotItem::StaticClass();
}

UInventoryItemSlot* UInventoryItemSlotTransactionBase::GetSelectedInventoryItemSlot() const {
	if(auto selectionClass = GetInventorySlotSelectionClass()){
		if (auto inventorySlotSelection = Cast<USelectInventorySlotItem>(GetContext().GetMerchant().GetSelectionByClass(selectionClass))) {
			return inventorySlotSelection->GetInventorySlot();
		}
	}
	return nullptr;
}

