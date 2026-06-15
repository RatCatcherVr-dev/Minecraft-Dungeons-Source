#include "Dungeons.h"
#include "ItemGenerator.h"
#include "ItemGeneratorLooterState.h"
#include "game/GameSettings.h"
#include "game/item/ItemType.h"
#include "game/Enchantments/generator/EnchantmentGenerator.h"
#include "util/SharedRandom.h"
#include "util/FloatWeighedRandom.h"
#include "util/Math.h"
#include "game/UniqueId.h"
#include "game/Enchantments/generator/EnchantmentGeneratorConfigs.h"
#include "util/CollectionUtils.h"
#include "ItemGeneratorConfigs.h"

namespace game { namespace item { namespace generator {

struct ItemChance {
	ItemChance(const ItemType& item, float probability)
		: type(item)
		, baseProbability(probability) {
	}
	float getWeight() const { return baseProbability; }
	float isPossible() const { return baseProbability > 0; }

	const ItemType& type;
	float baseProbability;
};

using ItemChances = std::vector<ItemChance>;

const ItemChances& defaultItemChances() {
	static const ItemChances itemChances = [&] {
		ItemChances out;
		for (auto& item : GetItemRegistry().GetValues()) {
			out.emplace_back( ItemChance{ *item, 1.0f } );
		}
		return out;
	}();
	return itemChances;
}

ItemChances calculateModifiedItemChances(const EnvState& env, const Config& config) {
	ItemChances chances = defaultItemChances();
	for (auto& chance : chances) {
		if (!config.pred({ chance.type, env })) { 
			chance.baseProbability = 0;
		} else {
			chance.baseProbability = config.probability({ chance.type, env });
		}
	}
	return chances;
}

// Note: I split up the creation from the enchanting since:
//   1) the enchantment doesn't seem trivial and
//   2) we only need enchantment ~10% of the time
const ItemType* generateType(const ItemChances& pickableItems) {
	const auto index = FloatWeighedRandom::getRandomItemIndex(&Util::sharedRandom(), pickableItems);
	return index >= 0 ? &pickableItems[index].type : nullptr;
}

FInventoryItemData generateFullItem(const ItemType& item, float power, TArray<FEnchantmentData> enchantments, EItemRarity rarity, TOptional<FEnchantmentData> netheriteEnchant, const TSet<ELevelNames>& unlockedLevels) {
	return FInventoryItemData(item.getId(), power, std::move(enchantments), netheriteEnchant, item.getDefaultArmorProperties(unlockedLevels), rarity, false, false);
}

FInventoryItemData generateFullItem(const ItemType& item, float power, const Config& config, const EnvState& env) {
	auto availableEnchantments = enchgen::getAvailableEnchantmentsForType(item.getId());
	auto netheriteEnchantment = [&]() -> TOptional<FEnchantmentData> {
		const auto netheriteCounts = config.netheriteEnchantment({ item, env });
		auto netheriteEnchantments = netheriteCounts.enchantments > 0 ? enchgen::generate(availableEnchantments, netheriteCounts, nullptr) : TArray<FEnchantmentData>();
		if (auto* enchantment = netheriteEnchantments.FindByPredicate(RETLAMBDA(it.TypeID != EEnchantmentTypeID::Unset))) {
			enchantment->Level = enchantmentlevelcalcs::GenerateNetheriteEnchantmentLevel(env.settings.difficulty);
			return *enchantment;
		}
		return {};
	}();
	if (netheriteEnchantment) {
		availableEnchantments.powerful.RemoveAllSwap(RETLAMBDA(it->getEnchantmentTypeID() == netheriteEnchantment->TypeID));
		availableEnchantments.nonPowerful.RemoveAllSwap(RETLAMBDA(it->getEnchantmentTypeID() == netheriteEnchantment->TypeID));
	}
	auto enchantments = enchgen::generate(availableEnchantments, config.enchantment({ item, env }));
	return generateFullItem(item, power, std::move(enchantments), EItemRarity::Common, netheriteEnchantment, env.looter.unlockedLevels);
}

TOptional<FInventoryItemData> generateFullItem(const ItemChances& pickableTypes, const Config& config, const EnvState& env) {
	if(const auto* item = generateType(pickableTypes)) {
		return generateFullItem(*item, config.power(config.powerrange({env })), config, env);
	}

	return {};
}

//Generate randomized item type using generator settings and an intended looter.
TOptional<FInventoryItemData> generate(const game::Settings& settings,	Looter looter, const Config& config) {
	const EnvState env{ settings, looter };
	const ItemChances allPossibleItems = calculateModifiedItemChances(env, config);
	ItemChances pickableItems = allPossibleItems;

	//for (int boost = 10; boost >= 0; --boost) {
	//	const ItemType& type = generateType(pickableItems);
	//	if (EItemType::Unset == type.getId()) {
	//		break;
	//	}
	//	FInventoryItemData item = { type.getId(), Math::roundTo(config.power({ type, env }), 0.01f) };
	//	pickableItems[enum_cast(item.ItemType)].baseProbability = 0; // We can do this because 'pickableItems' full size ( == EItemType::Last)

	//	if (item.ItemPower >= 0.1f * boost + looter.highestItemPowerOf(item.ItemType)) {
	//		return generateFullItem(type, item.ItemPower, config, env);
	//	}
	//}
	return generateFullItem(allPossibleItems, config, env);
}

TArray<FItemId> getPossibleItemTypeIds(const game::Settings& settings, Looter looter, const Config& config) {
	return algo::copy_if_map_tarray(calculateModifiedItemChances(EnvState{ settings, looter }, config),
		RETLAMBDA(it.isPossible()),
		RETLAMBDA(it.type.getId())
	);
}

FInventoryItemData generate(const ItemType& item, float power, TArray<FEnchantmentData> enchantments, EItemRarity rarity, TOptional<FEnchantmentData> netheriteEnchant, const TSet<ELevelNames>& unlockedLevels) {
	return generateFullItem(item, power, std::move(enchantments), rarity, netheriteEnchant, unlockedLevels);
}

//Generate specific item type with specified power, enchantments and rarity.
FInventoryItemData generate(FItemId itemType, float power, TArray<FEnchantmentData> enchantments, EItemRarity rarity, TOptional<FEnchantmentData> netheriteEnchant, const TSet<ELevelNames>& unlockedLevels) {
	const auto& item = GetItemRegistry().Get(itemType);
	return generateFullItem(item, power, std::move(enchantments), rarity, netheriteEnchant, unlockedLevels);
}

FInventoryItemData rerollPower(FInventoryItemData item, const game::Settings& settings, itemgen::Looter looter, const PowerRangeCalc& powerRangeCalc, const PowerCalc& powerCalc) {
	const EnvState env{ settings, looter };
	const auto powerRange = powerRangeCalc(env);
	item.ItemPower = powerCalc(powerRange);
	return item;
}

PowerRange getPowerRange(const game::Settings& settings, itemgen::Looter looter, const PowerRangeCalc& powerRangeCalc) {
	const EnvState env{ settings, looter };
	return powerRangeCalc(env);	
}

}}}
