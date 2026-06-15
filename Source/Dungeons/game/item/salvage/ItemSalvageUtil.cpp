#include "Dungeons.h"
#include "ItemSalvageUtil.h"
#include "game/inventory/InventoryItemUtil.h"
#include "game/item/InventoryItemData.h"
#include "game/item/ItemRarity.h"
#include "game/item/ItemTypeDefs.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/item/ItemType.h"

const float UItemSalvageUtil::SALVAGE_EMERALDS_POWER_FACTOR = 15.0f;
const float UItemSalvageUtil::SALVAGE_GOLD_POWER_FACTOR = 1.0f;

int32 UItemSalvageUtil::GetSalvageRarityMultiplier(const FInventoryItemData& item) {
	switch (item.Rarity) {
	case EItemRarity::Unique:
		return 5;
	case EItemRarity::Rare:
		return 2;
	case EItemRarity::Common:
	default:
		return 1;
	}
}

FItemSalvageInfo UItemSalvageUtil::GetSalvageItemInfo(const FInventoryItemData& item, const float valueMultiplier) {
	FItemSalvageInfo info;
	if (item.IsNetherite()) {
		const int32 gold = SALVAGE_GOLD_POWER_FACTOR * game::item::power::SalvageValueMultiplier(item.ItemPower) * GetSalvageRarityMultiplier(item) * valueMultiplier;
		info.currencies.Add(game::item::type::Gold.getId(), gold);
	}
	else {
		const int32 emeralds = SALVAGE_EMERALDS_POWER_FACTOR * game::item::power::SalvageValueMultiplier(item.ItemPower) * GetSalvageRarityMultiplier(item) * valueMultiplier;
		info.currencies.Add(game::item::type::Emerald.getId(), emeralds);
	}
	info.enchantmentPoints = UInventoryItemUtil::GetTotalInvestedEnchantmentPoints(item);
	return info;
}
