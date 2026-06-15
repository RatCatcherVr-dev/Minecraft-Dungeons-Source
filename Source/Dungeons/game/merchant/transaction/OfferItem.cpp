#include "Dungeons.h"
#include "game/merchant/slot/OfferedItemSlot.h"
#include "game/component/ItemStashComponent.h"
#include "OfferItem.h"

FMerchantTransactionStatus UOfferItem::Validate() const {
	auto slot = GetSelectedSlot<UOfferedItemSlot>();
	if (!slot) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::NO_SLOT_SELECTED) };
	}

	auto selectedSlot = GetSelectedInventoryItemSlot();
	if (!selectedSlot) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::NO_INVENTORY_SLOT_SELECTED) };
	}

	if (!selectedSlot->GetItem()) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::NO_INVENTORY_ITEM_SELECTED) };
	}

	auto merchant = GetMerchantMutable();
	auto selectionClass = GetInventorySlotSelectionClass();
	return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::OFFER_ITEM), {}, {},
		FMerchantTransactionStatus::ValidatedTransaction([selectionClass, merchant, slot, selectedSlot]() -> void {
			slot->SetInventoryItem(selectedSlot->GetItem());
			if (auto selection = merchant->GetSelectionByClass(selectionClass)) {
				selection->TryCancelSelection();
			}
		})
	};
}

EMerchantTransactionStatusReason UOfferItem::SuccessReason() const {
	return EMerchantTransactionStatusReason::OFFER_ITEM;
}

