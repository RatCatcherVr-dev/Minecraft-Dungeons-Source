#include "Dungeons.h"
#include "game/component/ItemStashComponent.h"
#include "game/component/PickupItemComponent.h"
#include "game/component/EquipmentComponent.h"
#include "ClaimAdventureHubReward.h"
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

FMerchantTransactionStatus UClaimAdventureHubReward::Validate() const
{
	return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::COLLECT_ITEM), {}, {} };
}

EMerchantTransactionStatusReason UClaimAdventureHubReward::SuccessReason() const {
	return EMerchantTransactionStatusReason::CLAIM_HUB_ITEM;
}

