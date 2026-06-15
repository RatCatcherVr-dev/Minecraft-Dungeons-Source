#include "Dungeons.h"
#include "game/component/ItemStashComponent.h"
#include "game/component/EquipmentComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/difficulty/DifficultyStats.h"
#include "game/merchant/selection/SelectOtherPlayerCharacter.h"
#include "game/merchant/selection/SelectInventorySlotItem.h"
#include "game/item/generator/ItemGeneratorConfigs.h"
#include "game/item/generator/ItemGeneratorLooterState.h"
#include "game/item/generator/ItemGenerator.h"
#include "game/merchant/type/MerchantBase.h"
#include "game/merchant/announcement/ItemAnnouncement.h"
#include "game/merchant/slot/MerchantItemSlotBase.h"
#include "game/Game.h"
#include "game/GameSettings.h"
#include "GiftWrapItem.h"


APlayerCharacter* UGiftWrapItem::GetSelectedRecipient() const {
	if (auto playerSelection = GetContext().GetMerchant().GetSelection<USelectOtherPlayerCharacter>()) {
		return playerSelection->GetPlayerCharacter();
	}
	return nullptr;
}


void UGiftWrapItem::PopulateRequiredSelectionClasses(TSet<TSubclassOf<UMerchantSelectionBase>>& selections) const {
	selections.Add(USelectOtherPlayerCharacter::StaticClass());
	selections.Add(USelectInventorySlotItem::StaticClass());
}

FMerchantTransactionStatus UGiftWrapItem::Validate() const
{
	auto giftBoxSlot = GetSelectedSlot<UMerchantItemSlotBase>();
	if (!giftBoxSlot) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::NO_SLOT_SELECTED) };
	}

	auto maybeGiftBox = giftBoxSlot->GetOptionalItem();
	if (!maybeGiftBox.IsSet()) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::NO_ITEM_SELECTED) };
	}

	if (!giftBoxSlot->IsItemUnlocked()) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::ITEM_LOCKED) };
	}

	auto inventorySlot = GetSelectedInventoryItemSlot();
	if (!inventorySlot) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::NO_INVENTORY_SLOT_SELECTED) };
	}

	auto* maybeItemToGift = inventorySlot->GetItem();
	if (!maybeItemToGift) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::NO_INVENTORY_ITEM_SELECTED) };
	}

	if (maybeItemToGift->IsGifted()) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::NO_REGIFTING) };
	}

	if (maybeItemToGift->IsCloned()) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::CANT_GIFT_CLONES) };
	}

	auto session = GetSession();

	auto itemDataToGift = maybeItemToGift->GetItemData();
	auto maybePrice = session.GetGiftPrice(itemDataToGift);

	
	auto recipient = GetSelectedRecipient();
	if (!recipient) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::NO_PLAYER_SELECTED), maybePrice };
	}

	if (recipient == &session.GetShopper()) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::NO_GIFTING_TO_SELF), maybePrice };
	}

	auto recipientStash = recipient->GetItemStashComponent();
	if (!recipientStash || recipientStash->IsInventoryFull()) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::RECIPIENT_INVENTORY_FULL), maybePrice };
	}

	if(maybePrice.IsSet()){
		if (!session.CanAfford(maybePrice.GetValue())) {
			return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::CANT_AFFORD), maybePrice };
		}
	}

	return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::GIFT_ITEM), maybePrice, {},
		FMerchantTransactionStatus::ValidatedTransaction([giftBoxSlot, inventorySlot, itemDataToGift, recipient, recipientStash, maybePrice, session, context = GetContext()]() -> void {
			auto giftBox = giftBoxSlot->PopItem();
			auto ownerStash = session.GetItemStashComponentMutable();
			if(ownerStash->RemoveItem(inventorySlot)){

				auto giftedDataCopy = itemDataToGift;

				giftedDataCopy.bIsGifted = true;
				giftedDataCopy.ResetEnchantmentLevels();
				ownerStash->ServerVendorGiftItemTo(recipient, giftedDataCopy);
				session.GetMerchant().BroadcastItemAnnouncement({ EItemAnnouncementType::GIFT_SENT, giftedDataCopy });
				if (maybePrice.IsSet()) {
					session.Deduct(maybePrice.GetValue());
				}
				context.IncrementProgressStat(EProgressStat::GIVE_GIFTS);
			}
		})
	};
}

EMerchantTransactionStatusReason UGiftWrapItem::SuccessReason() const {
	return EMerchantTransactionStatusReason::GIFT_ITEM;
}


bool UGiftWrapItem::rerollItemPower_LocalOnly(AActor* localRecipient, FInventoryItemData& item) {
	if (APlayerCharacter* localPlayerRecipient = Cast<APlayerCharacter>(localRecipient)) {
		check(localPlayerRecipient->IsLocallyControlled() && "this only works on local characters");
		if (auto game = actorquery::getGame(localPlayerRecipient->GetWorld())) {
			auto currentSettings = game->settings();
			auto recommendedSettings = game::Settings(localPlayerRecipient->GetEquipmentComponent()->GetDifficultyRecommendation().getDifficulty(EExtraChallenge::NoExtraChallenge), currentSettings.randomSeed, currentSettings.levelName);
			item = itemgen::rerollPower(item, recommendedSettings, itemgen::looters::FromPlayerCharacter(*localPlayerRecipient), itemgen::powerrangecalcs::FromVendorRarity(item.Rarity), itemgen::powercalcs::Default());
			return true;
		}
	}
	return false;
}
