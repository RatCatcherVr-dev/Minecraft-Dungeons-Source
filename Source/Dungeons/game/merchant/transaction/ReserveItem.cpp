#include "Dungeons.h"
#include "game/merchant/type/MerchantBase.h"
#include "game/merchant/slot/MerchantItemSlotBase.h"
#include "game/component/ShopperComponent.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "ReserveItem.h"

FMerchantTransactionStatus UReserveItem::Validate() const
{
	auto slot = GetSelectedSlot<UMerchantItemSlotBase>();
	if (!slot) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::NO_SLOT_SELECTED) };
	}
	
	if (!slot->HasItem()) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::NO_ITEM_SELECTED) };
	}

	if (slot->IsReserved()) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::ALREADY_RESERVED) };
	}	

	return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::RESERVE_ITEM), {}, {},
		FMerchantTransactionStatus::ValidatedTransaction([slot, session = GetSession()]() -> void {
			for (auto& otherSlot : session.GetMerchant().GetSlots()) {
				if (auto itemSlot = Cast<UMerchantItemSlotBase>(otherSlot)) {
					if (itemSlot != slot) {
						itemSlot->SetReserved(false);
					}
				}
			}
			slot->SetReserved(true);

			if (auto controller = session.GetShopper().GetPlayerController()) {
				if (auto shopperComponent = controller->FindComponentByClass<UShopperComponent>()) {
					shopperComponent->OnItemReserved.Broadcast();
				}
			}

		})
	};
}

EMerchantTransactionStatusReason UReserveItem::SuccessReason() const {
	return EMerchantTransactionStatusReason::RESERVE_ITEM;
}
