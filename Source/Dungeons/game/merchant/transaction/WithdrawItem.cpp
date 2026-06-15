#include "Dungeons.h"
#include "game/merchant/slot/OfferedItemSlot.h"
#include "WithdrawItem.h"

FMerchantTransactionStatus UWithdrawItem::Validate() const {
	auto slot = GetSelectedSlot<UOfferedItemSlot>();
	if (!slot) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::NO_SLOT_SELECTED) };
	}

	if (!slot->HasInventoryItem()) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::NOTHING_TO_WITHDRAW) };
	}

	return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::WITHDRAW_ITEM), {}, {},
		FMerchantTransactionStatus::ValidatedTransaction([slot]() -> void {
			slot->ClearInventoryItem();
		})
	};
}

EMerchantTransactionStatusReason UWithdrawItem::SuccessReason() const {
	return EMerchantTransactionStatusReason::WITHDRAW_ITEM;
}

