#include "Dungeons.h"
#include "MobEnchantmentGenerator.h"
#include "MobEnchantmentTypes.h"
#include "MobEnchantmentUtil.h"
#include "game/Enchantments/generator/EnchantmentGenerator.h"
#include "game/Enchantments/generator/EnchantmentGeneratorTypes.h"
#include "game/Enchantments/generator/EnchantmentGeneratorUtil.h"
#include "game/Enchantments/EnchantmentUtil.h"
#include "util/Algo.h"
#include "util/CollectionUtils.h"

namespace game { namespace enchantment { namespace mob { namespace generator {

TArray<FEnchantmentData> generate(EntityType type, const mobench::EnchantmentCounts& counts, const FDifficulty& difficulty, Random* rnd /*= nullptr*/) {
	const auto availableEnchantments = getAvailableEnchantmentsForMob(type, difficulty);
	const auto enchantmentCounts = enchgen::Counts(1, 1, counts.getEnchantments(), 1, counts.getPowerful());
	const auto pickedEnchantments = enchgen::generate(availableEnchantments, enchantmentCounts, rnd);

	const auto validEnchantments = algo::copy_if(pickedEnchantments, RETLAMBDA(it.TypeID != EEnchantmentTypeID::Unset));

	return Util::map(RETLAMBDA(FEnchantmentData(it.TypeID, 3)), validEnchantments);
}

}}}}
