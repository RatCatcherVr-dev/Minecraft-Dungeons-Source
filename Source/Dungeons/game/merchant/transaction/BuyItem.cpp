#include "Dungeons.h"
#include "DungeonsGameInstance.h"
#include "game/component/PickupItemComponent.h"
#include "game/component/ItemStashComponent.h"
#include "game/inventory/InventoryItem.h"
#include "game/Game.h"
#include "game/merchant/slot/MerchantItemSlotBase.h"
#include "game/merchant/announcement/ItemAnnouncement.h"
#include "game/merchant/type/MerchantBase.h"
#include "BuyItem.h"
#include "Util/telemetry/Analytics.h"

FMerchantTransactionStatus UBuyItem::Validate() const
{
	auto slot = GetSelectedSlot<UMerchantItemSlotBase>();
	if (!slot) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::NO_SLOT_SELECTED) };
	}

	auto maybeItem = slot->GetOptionalItem();
	if (!maybeItem.IsSet()) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::NO_ITEM_SELECTED) };
	}
		
	if (!slot->IsItemUnlocked()) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::ITEM_LOCKED) };
	}

	auto maybePrice = slot->GetOptionalPrice();
	if(maybePrice.IsSet()){
		if(!GetSession().CanAfford(maybePrice.GetValue())){
			return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::CANT_AFFORD), maybePrice };
		}
	}

	if (GetSession().GetItemStashComponent()->IsInventoryFull()) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::INVENTORY_FULL), maybePrice };
	}

	return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::BUY_ITEM), maybePrice, {},
		FMerchantTransactionStatus::ValidatedTransaction([slot, maybePrice, session = GetSession(), world = GetWorld()]() -> void {
			auto* pickuper = session.GetPickupItemComponentMutable();
			auto purchasedItemData = slot->PopItem();
			purchasedItemData.OnAdded.BindLambda([session](UInventoryItem* item) -> void {
				session.GetMerchant().BroadcastItemAnnouncement({ EItemAnnouncementType::PURCHASED, item->Item });
			});
			pickuper->Pickup(purchasedItemData);
			if (maybePrice.IsSet()) {
				session.Deduct(maybePrice.GetValue());
			}
			if (auto gameInstance = world->GetGameInstance<UDungeonsGameInstance>()) {
				analytics::Analytics::GetInstance().FireEventMerchantPurchase(session.GetShopper(), session.GetMerchant(), purchasedItemData, maybePrice);
			}
		})
	};
}

EMerchantTransactionStatusReason UBuyItem::SuccessReason() const {
	return EMerchantTransactionStatusReason::BUY_ITEM;
}

