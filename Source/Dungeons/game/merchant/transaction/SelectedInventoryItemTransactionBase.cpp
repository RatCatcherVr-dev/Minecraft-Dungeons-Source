#include "Dungeons.h"
#include "game/merchant/slot/InventoryItemMerchantSlotBase.h"
#include "SelectedInventoryItemTransactionBase.h"
#include "game/merchant/selection/SelectMerchantSlot.h"
#include "game/merchant/selection/SelectInventorySlotItem.h"
#include "game/merchant/MerchantContext.h"

TSubclassOf<USelectInventorySlotItem> USelectedInventoryItemTransactionBase::GetInventorySlotSelectionClass() const {
	if (auto slot = GetSelectedSlot<UInventoryItemMerchantSlotBase>()) {
		if (auto selection = slot->GetSelectionClass()){
			return selection.Get();
		}
	}
	return nullptr;
}
