#pragma once

#include "game/item/ItemType.h"
#include "game/item/ItemTypeDefs.h"
#include "game/item/enchantment/EnchantmentData.h"
#include "ItemGeneratorLooterState.h"
#include "ItemGeneratorTypes.h"
#include <Array.h>


namespace game {
struct Settings;


namespace item { namespace generator {

TOptional<FInventoryItemData> generate(const game::Settings&, itemgen::Looter, const Config&);
TArray<FItemId> getPossibleItemTypeIds(const game::Settings&, itemgen::Looter, const Config&);

FInventoryItemData generate(FItemId type, float power = 1.0f, TArray<FEnchantmentData> enchantments = {}, EItemRarity rarity = EItemRarity::Common, TOptional<FEnchantmentData> netheriteEnchant = {}, const TSet<ELevelNames>& unlockedLevels = {});
FInventoryItemData generate(const ItemType& type, float power = 1.0f, TArray<FEnchantmentData> enchantments = {}, EItemRarity rarity = EItemRarity::Common, TOptional<FEnchantmentData> netheriteEnchant = {}, const TSet<ELevelNames>& unlockedLevels = {});
FInventoryItemData rerollPower(FInventoryItemData, const game::Settings&, itemgen::Looter, const PowerRangeCalc&, const PowerCalc&);
PowerRange getPowerRange(const game::Settings&, itemgen::Looter, const PowerRangeCalc&);

}}}
