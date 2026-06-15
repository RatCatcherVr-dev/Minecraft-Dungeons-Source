#include "Dungeons.h"
#include "AffectorEnchantmentUtil.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/component/MeleeAttackComponent.h"
#include "game/component/RangedAttackComponent.h"
#include "util/CollectionUtils.h"

namespace affector {

bool EnchantmentCategoryCollector::isExplicitlyExcluded(const AMobCharacter& mob, EEnchantmentCategory category) {
	static const auto excluded = []() {
		TSet<TPair<EntityType, EEnchantmentCategory>> out {
			Util::makeTPair(EntityType::ArchVisage, EEnchantmentCategory::Melee),
			Util::makeTPair(EntityType::Creeper, EEnchantmentCategory::Melee),
			Util::makeTPair(EntityType::Enchanter, EEnchantmentCategory::Melee),
			Util::makeTPair(EntityType::Geomancer, EEnchantmentCategory::Melee),
			Util::makeTPair(EntityType::IcyCreeper, EEnchantmentCategory::Melee),
			Util::makeTPair(EntityType::JackOLantern, EEnchantmentCategory::Melee),
			Util::makeTPair(EntityType::Llama, EEnchantmentCategory::Melee),
			Util::makeTPair(EntityType::Pillager, EEnchantmentCategory::Melee),
			Util::makeTPair(EntityType::PillagerVariant0, EEnchantmentCategory::Melee),
			Util::makeTPair(EntityType::PillagerVariant1, EEnchantmentCategory::Melee),
			Util::makeTPair(EntityType::PillagerVariant2, EEnchantmentCategory::Melee),
			Util::makeTPair(EntityType::SheepFireRed, EEnchantmentCategory::Melee),
			Util::makeTPair(EntityType::SheepSpeedBlue, EEnchantmentCategory::Melee),
			Util::makeTPair(EntityType::SlimeCauldron, EEnchantmentCategory::Melee),
			Util::makeTPair(EntityType::SoulWizard, EEnchantmentCategory::Melee),
			Util::makeTPair(EntityType::Whisperer, EEnchantmentCategory::Melee),
			Util::makeTPair(EntityType::WickedWraith, EEnchantmentCategory::Melee),
			Util::makeTPair(EntityType::Witch, EEnchantmentCategory::Melee),
			Util::makeTPair(EntityType::Wraith, EEnchantmentCategory::Melee),
		};
		return out;
	}();
	if (excluded.Contains(Util::makeTPair(mob.EntityType, category))) {
		return true;
	}
	return false;
}

TArray<EEnchantmentCategory> EnchantmentCategoryCollector::getCategories(const AMobCharacter& mob) const {
	TArray<EEnchantmentCategory> categories;

	if (!isExplicitlyExcluded(mob, EEnchantmentCategory::Armor)) {
		categories.Add(EEnchantmentCategory::Armor);
	}
	if (!isExplicitlyExcluded(mob, EEnchantmentCategory::Melee)) {
		const auto* melee = mob.FindComponentByClass<UMeleeAttackComponent>();
		if (melee && melee->GetActiveAttackVariants().Num() > 0) {
			categories.Add(EEnchantmentCategory::Melee);
		}
	}
	if (!isExplicitlyExcluded(mob, EEnchantmentCategory::Ranged) && mob.FindComponentByClass<URangedAttackComponent>()) {
		categories.Add(EEnchantmentCategory::Ranged);
	}
	return categories;
}

}
