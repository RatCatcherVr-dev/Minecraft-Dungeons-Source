#include "Dungeons.h"
#include "MobEnchantmentTypes.h"
#include "game/GameTypes.h"
#include "game/component/MeleeAttackComponent.h"
#include "game/component/RangedAttackComponent.h"
#include "game/Enchantments/generator/EnchantmentGenerator.h"
#include "game/Enchantments/generator/EnchantmentGeneratorTypes.h"
#include "game/Enchantments/generator/EnchantmentGeneratorUtil.h"
#include "game/Enchantments/EnchantmentUtil.h"
#include "game/item/ItemType.h"
#include "game/util/ComponentUtils.h"
#include "util/Algo.h"

namespace game { namespace enchantment { namespace mob {

TArray<EEnchantmentCategory> getAvailableCategoriesForType(EntityType type) {
	auto defaultClass = TypeMap::singleton().mobClass(type);

	TArray<EEnchantmentCategory> out{ EEnchantmentCategory::Armor };

	TArray<UMeleeAttackComponent*> meleeComps;
	componentutils::FindDefaultComponentsByClass<UMeleeAttackComponent>(defaultClass, UMeleeAttackComponent::StaticClass(), meleeComps);

	if (meleeComps.Num()) {
		if (meleeComps[0]->GetActiveAttackVariants().Num()) {
			out.Emplace(EEnchantmentCategory::Melee);
		}
	}

	TArray<URangedAttackComponent*> rangedComps;
	componentutils::FindDefaultComponentsByClass<URangedAttackComponent>(defaultClass, URangedAttackComponent::StaticClass(), rangedComps);

	if (rangedComps.Num()) {
		out.Emplace(EEnchantmentCategory::Ranged);
	}

	return out;
}

// @TODO: probably move this to some util header
EEnchantmentCategory fromArray(const TArray<EEnchantmentCategory>& categories) {
	EEnchantmentCategory category = EEnchantmentCategory::Unset;
	algo::for_each(categories, [&category](auto c) {category |= c; });
	return category;
}

bool isEnchantmentApplicableFor(EEnchantmentTypeID enchantmentType, const TArray<EEnchantmentCategory>& availableCategories, const FDifficulty& difficulty) {
	const auto& enchantment = type::getEnchantmentType(enchantmentType);

	if (enchantment.isWorkInProgress()) {
		return false;
	}
	if (!enchantment.canBeUsedByMobs()) {
		return false;
	}
	if (enchantment.hasTag(EEnchantmentTag::SoulUsage)) {
		return false;
	}
	auto categoryFilter = fromArray(availableCategories);
	auto category = enchantment.getEnchantmentCategory();
	if (!(category & categoryFilter))
		return false;
	//if (!availableCategories.Contains(enchantment.getEnchantmentCategory())) {
	//	return false;
	//}
	if (enchantment.difficultyThreshold().combinedFraction() > difficulty.combinedFraction()) {
		return false;
	}
	return true;
}

enchgen::AvailableEnchantments getAvailableEnchantmentsForMob(EntityType entityType, const FDifficulty& difficulty) {
	auto availableCategories = getAvailableCategoriesForType(entityType);

	enchgen::AvailableEnchantments out;
	for (const auto& enchantment : type::getAvailableEnchantments()) {
		if (isEnchantmentApplicableFor(enchantment.getEnchantmentTypeID(), availableCategories, difficulty)) {
			(enchantment.isRarity(EEnchantmentRarity::Powerful) ? out.powerful : out.nonPowerful).Add(&enchantment);
		}
	}
	return out;
}

}}}
