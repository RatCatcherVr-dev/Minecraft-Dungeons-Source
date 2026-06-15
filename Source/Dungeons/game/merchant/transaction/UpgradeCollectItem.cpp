#include "Dungeons.h"
#include "game/component/ItemStashComponent.h"
#include "game/component/PickupItemComponent.h"
#include "game/component/EquipmentComponent.h"
#include "UpgradeCollectItem.h"
#include "game/difficulty/DifficultyStats.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/inventory/InventoryItem.h"
#include "game/item/generator/ItemGenerator.h"
#include "game/item/generator/ItemGeneratorLooterState.h"
#include "game/item/generator/ItemGeneratorConfigs.h"
#include "game/item/drop/ItemDropGenerator.h"
#include "game/item/InventoryItemData.h"
#include "game/merchant/slot/MerchantItemSlotBase.h"
#include "game/merchant/announcement/ItemAnnouncement.h"
#include "game/merchant/MerchantContext.h"
#include "game/item/power/ItemPowerUtil.h"

FMerchantTransactionStatus UUpgradeCollectItem::Validate() const
{
	auto slot = GetSelectedSlot<UMerchantItemSlotBase>();
	if (!slot) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::NO_SLOT_SELECTED) };
	}

	if (!slot->HasItem()) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::NO_ITEM_SELECTED) };
	}

	auto session = GetSession();
	auto item = slot->GetItem();
	auto localSettings = session.GetGameSettings();
	auto recommendedSettings = game::Settings(session.GetDifficultyRecommendation().getDifficulty(EExtraChallenge::NoExtraChallenge), localSettings.randomSeed, localSettings.levelName);
	const auto looter = itemgen::looters::FromPlayerCharacter(session.GetShopper());
	const auto& powerRangeCalc = game::item::drop::getPowerRangeCalc(item.Rarity, itemgen::ItemSource::Merchant);
	auto powerRange = itemgen::getPowerRange(recommendedSettings, looter, powerRangeCalc);
	const float currentItemPower = item.ItemPower;
	const float currentPlusVisualOneItemPower = UItemPowerUtil::GetItemPowerFromDisplayValue(UItemPowerUtil::GetItemPowerDisplayValueFloat(item.ItemPower) + 1);
	auto powerRangeLimit = itemgen::PowerRange(FMath::Min(powerRange.max(), FMath::Max(powerRange.min(), currentPlusVisualOneItemPower)), powerRange.max());

	{
		TArray<FItemBulletPoint> bullets = { FItemBulletPoint::CreateEstimatedItemPowerRangeBulletPoint(powerRangeLimit) };

		if (!slot->IsItemUnlocked()) {
			return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::ITEM_LOCKED), {}, bullets };
		}

		if (session.GetItemStashComponent()->IsInventoryFull()) {
			return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::INVENTORY_FULL), {}, bullets };
		}
	}

	TArray<FItemBulletPoint> bullets = { FItemBulletPoint::CreateUpgradedItemPowerRangeBulletPoint(powerRangeLimit) };
	return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::UPGRADE_COLLECT_ITEM), {}, bullets,
		FMerchantTransactionStatus::ValidatedTransaction([slot, session, &powerRangeCalc, powerRangeLimit, looter, recommendedSettings, currentItemPower]() -> void {
			
			auto* pickuper = session.GetPickupItemComponentMutable();

			auto collectedItem = slot->PopItem();			

			const auto& powerCalc = itemgen::powercalcs::Default();
			auto rerolledItemData = itemgen::rerollPower(collectedItem, recommendedSettings, looter, powerRangeCalc, powerCalc);
			
			rerolledItemData.ItemPower = FMath::Max(currentItemPower, FMath::Clamp(rerolledItemData.ItemPower, powerRangeLimit.min(), powerRangeLimit.max()));
			rerolledItemData.OnAdded.BindLambda([session](UInventoryItem* item) -> void {
				session.GetMerchant().BroadcastItemAnnouncement({ EItemAnnouncementType::UPGRADED, item->Item });
			});
			pickuper->Pickup(rerolledItemData);			
		})
	};
}

EMerchantTransactionStatusReason UUpgradeCollectItem::SuccessReason() const {
	return EMerchantTransactionStatusReason::UPGRADE_COLLECT_ITEM;
}

