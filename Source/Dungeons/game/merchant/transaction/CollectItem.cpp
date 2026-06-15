#include "Dungeons.h"
#include "game/component/ItemStashComponent.h"
#include "game/component/PickupItemComponent.h"
#include "game/component/ShopperComponent.h"
#include "game/inventory/InventoryItem.h"
#include "game/merchant/slot/MerchantItemSlotBase.h"
#include "game/merchant/announcement/ItemAnnouncement.h"
#include "CollectItem.h"

FMerchantTransactionStatus UCollectItem::Validate() const
{
	auto slot = GetSelectedSlot<UMerchantItemSlotBase>();
	if (!slot) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::NO_SLOT_SELECTED) };
	}

	if (!slot->HasItem()) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::NO_ITEM_SELECTED) };
	}

	if (!slot->IsItemUnlocked()) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::ITEM_LOCKED) };
	}

	if (GetSession().GetItemStashComponent()->IsInventoryFull()) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::INVENTORY_FULL) };
	}

	return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::COLLECT_ITEM), {}, {},
		FMerchantTransactionStatus::ValidatedTransaction([slot, session = GetSession()]() -> void {
			auto pickuper = session.GetPickupItemComponentMutable();
			auto collectedItem = slot->PopItem();			
			collectedItem.OnAdded.BindLambda([session](UInventoryItem* item) -> void {
				session.GetMerchant().BroadcastItemAnnouncement({ EItemAnnouncementType::COLLECTED, item->Item });
			});
			pickuper->Pickup(collectedItem);
		})
	};
}

EMerchantTransactionStatusReason UCollectItem::SuccessReason() const {
	return EMerchantTransactionStatusReason::COLLECT_ITEM;
}

