#include "ModifyItem.h"

#include "Dungeons.h"
#include "game/component/ItemStashComponent.h"
#include "game/Enchantments/generator/EnchantmentGenerator.h"
#include "game/Enchantments/generator/EnchantmentGeneratorConfigs.h"
#include "game/merchant/selection/SelectInventorySlotItemEnchantable.h"
#include "game/Game.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/item/drop/ItemDropGenerator.h"
#include "game/item/InventoryItemData.h"
#include "util/SharedRandom.h"

void UModifyItem::PopulateRequiredSelectionClasses(TSet<TSubclassOf<UMerchantSelectionBase>>& selections) const {
	selections.Add(USelectInventorySlotItemEnchantable::StaticClass());
}

TArray<int32> UModifyItem::AvailableEnchantmentIndexes(const UInventoryItem* maybeItem) {
	TArray<int32> enchantmentIndexes;
	const auto maxIndex = maybeItem->Item.Enchantments.Num();
	for (int32 index = 0; index < maxIndex; index++)
	{
		if (!maybeItem->IsEnchantIndexHidden(index)) {
			enchantmentIndexes.Add(index);
		}
	}

	return enchantmentIndexes;
}

TOptional<int32> UModifyItem::GetRandomSelectedEnchantmentIndex(const UInventoryItem* maybeItem) {
	auto enchantmentIndexes = AvailableEnchantmentIndexes(maybeItem);
	if (enchantmentIndexes.Num() < 1) {
		return {};
	}

	const auto randomChoice = Util::sharedRandom().nextInt(enchantmentIndexes.Num());
	return enchantmentIndexes[randomChoice];
}

TOptional<FEnchantmentData> UModifyItem::GenerateReplacementEnchantment(UInventoryItem* maybeItem, const merchant::Session session) const
{
	const auto& itemData = maybeItem->GetItemData();

	// Generate a list of available enchantments for the selected item
	auto availableEnchantments = enchgen::getAvailableEnchantmentsForType(itemData.GetItemId());
	for (auto itemEnchantment : itemData.AllEnchantments()) {
		availableEnchantments.powerful.RemoveAllSwap(RETLAMBDA(it->getEnchantmentTypeID() == itemEnchantment.TypeID));
		availableEnchantments.nonPowerful.RemoveAllSwap(RETLAMBDA(it->getEnchantmentTypeID() == itemEnchantment.TypeID));
	}
	if (!availableEnchantments.powerful.Num() && !availableEnchantments.nonPowerful.Num()) {
		return {};
	}

	// Select the first available enchantment to set on the item. This will be selected from a slot in the future.
	const auto settings = game::item::drop::getSettings(session.GetShopper().GetWorld());
	const auto count = enchgen::configs::ReEnchantFromDifficulty(settings.difficulty, itemData.GetItemType().isUnique());
	auto enchantments = enchgen::generate(availableEnchantments, count);
	if (const auto maybeEnchantment = enchantments.FindByPredicate(RETLAMBDA(it.TypeID != EEnchantmentTypeID::Unset))) {
		return *maybeEnchantment;
	}

	return {};
}

FMerchantTransactionStatus UModifyItem::Validate() const
{
	const auto inventorySlot = GetSelectedInventoryItemSlotByClass<USelectInventorySlotItemEnchantable>();
	if (!inventorySlot) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::NO_INVENTORY_SLOT_SELECTED) };
	}

	auto* maybeItem = inventorySlot->GetItem();
	if (!maybeItem) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::NO_INVENTORY_ITEM_SELECTED) };
	}

	auto enchantmentIndex = GetRandomSelectedEnchantmentIndex(maybeItem);
	if (!enchantmentIndex.IsSet()) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::NO_ENCHANTMENT_INDEX_SELECTED) };
	}

	auto session = GetSession();

	auto maybePrice = session.GetPrice(maybeItem->GetItemData());
	if (maybePrice.IsSet()) {
		if (!session.CanAfford(maybePrice.GetValue())) {
			return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::CANT_AFFORD), maybePrice };
		}
	}

	auto maybeEnchantment = GenerateReplacementEnchantment(maybeItem, session);
	if (!maybeEnchantment.IsSet()) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::NO_NEW_ENCHANTMENTS_AVAILABLE), maybePrice };
	}

	return {
		FMerchantExecuteStatus(SuccessReason()),
		maybePrice, // If we want another price calculation, create a new function in UProgressPowerRarityPricing
		{}, // If we want some bullet points we can send them here
		FMerchantTransactionStatus::ValidatedTransaction(
			[maybeItem, maybePrice, enchantmentIndex, maybeEnchantment, session]() -> void {
				
				maybeItem->Item.ResetEnchantmentLevels();

				if(maybeItem->ReplaceEnchantment(enchantmentIndex.GetValue(), maybeEnchantment.GetValue()))
				{
					if (maybePrice.IsSet()) {
                        session.Deduct(maybePrice.GetValue());
                    }
				}
			}
		)
	};
}

EMerchantTransactionStatusReason UModifyItem::SuccessReason() const {
	return EMerchantTransactionStatusReason::MODIFY_ITEM;
}
