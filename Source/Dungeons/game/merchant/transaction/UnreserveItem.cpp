#include "Dungeons.h"
#include "game/merchant/slot/MerchantItemSlotBase.h"
#include "UnreserveItem.h"

FMerchantTransactionStatus UUnreserveItem::Validate() const
{
	auto slot = GetSelectedSlot<UMerchantItemSlotBase>();
	if (!slot) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::NO_SLOT_SELECTED) };
	}

	if (!slot->HasItem()) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::NO_ITEM_SELECTED) };
	}

	if (!slot->IsReserved()) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::NOT_RESERVED) };
	}

	return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::UNRESERVE_ITEM), {}, {},
		FMerchantTransactionStatus::ValidatedTransaction([slot]() -> void {
			slot->SetReserved(false);
		})
	};
}

EMerchantTransactionStatusReason UUnreserveItem::SuccessReason() const {
	return EMerchantTransactionStatusReason::UNRESERVE_ITEM;
}
