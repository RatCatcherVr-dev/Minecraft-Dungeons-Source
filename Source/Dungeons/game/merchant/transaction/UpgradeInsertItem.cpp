#include "Dungeons.h"
#include "game/component/ItemStashComponent.h"
#include "game/merchant/selection/SelectInventorySlotItem.h"
#include "game/difficulty/DifficultyStats.h"
#include "game/merchant/slot/MerchantItemSlotBase.h"
#include "UpgradeInsertItem.h"

void UUpgradeInsertItem::PopulateRequiredSelectionClasses(TSet<TSubclassOf<UMerchantSelectionBase>>& selections) const {
	selections.Add(USelectInventorySlotItem::StaticClass());
}

FMerchantTransactionStatus UUpgradeInsertItem::Validate() const
{
	auto slot = GetSelectedSlot<UMerchantItemSlotBase>();
	if (!slot) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::NO_SLOT_SELECTED) };
	}

	if (!slot->IsUnlocked()) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::SLOT_LOCKED) };
	}

	auto& currentItem = slot->GetOptionalItem();
	if (currentItem.IsSet()) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::SLOT_OCCUPIED) };
	}

	auto inventorySlot = GetSelectedInventoryItemSlot();
	if (!inventorySlot) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::NO_INVENTORY_SLOT_SELECTED) };
	}

	auto* maybeItem = inventorySlot->GetItem();
	if (!maybeItem) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::NO_INVENTORY_ITEM_SELECTED) };
	}

	auto session = GetSession();

	auto toUpgradeItemData = maybeItem->GetItemData();
	auto upgradeDeltaItemDataCopy = toUpgradeItemData;
	
	upgradeDeltaItemDataCopy.ItemPower = [&session](){
		if (auto& maybeHighestCompleted = session.GetHighestCompletedDifficultyStats()) {
			return maybeHighestCompleted.GetValue().GetCombinedItemPowerRange().max();
		}
		return game::DifficultyStats(game::FDifficulty::LOWEST).GetCombinedItemPowerRange().max();
	}();

	auto maybePrice = session.GetUpgradePrice(toUpgradeItemData, upgradeDeltaItemDataCopy);
	if(maybePrice.IsSet()){
		if (!session.CanAfford(maybePrice.GetValue())) {
			return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::CANT_AFFORD), maybePrice };
		}
	}

	auto selectionClass = GetInventorySlotSelectionClass();
	auto merchant = GetMerchant();
	return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::UPGRADE_ITEM), maybePrice, {},
		FMerchantTransactionStatus::ValidatedTransaction([selectionClass, merchant, slot, inventorySlot, toUpgradeItemData, maybePrice, session]() -> void {
			if(session.GetItemStashComponentMutable()->RemoveItem(inventorySlot)){
				auto toInsertUpgradeItemData = toUpgradeItemData;
				toInsertUpgradeItemData.ResetEnchantmentLevels();
				slot->SetItem(toInsertUpgradeItemData);
				if (maybePrice.IsSet()) {
					session.Deduct(maybePrice.GetValue());
				}

				if (auto selection = merchant->GetSelectionByClass(selectionClass)) {
					selection->TryCancelSelection();
				}
			}
		})
	};
}

EMerchantTransactionStatusReason UUpgradeInsertItem::SuccessReason() const {
	return EMerchantTransactionStatusReason::UPGRADE_ITEM;
}
