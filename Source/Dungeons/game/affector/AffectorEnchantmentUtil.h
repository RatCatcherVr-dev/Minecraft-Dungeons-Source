#pragma once

#include "game/Enchantments/EnchantmentType.h"

class AMobCharacter;

namespace affector {

// We're creating our own enchantment category collector here, since the one used for regularly
// enchanted mobs is broken (it seems to only detect BP-added (or non-inherited) components
struct EnchantmentCategoryCollector {
	TArray<EEnchantmentCategory> getCategories(const AMobCharacter&) const;
private:
	static bool isExplicitlyExcluded(const AMobCharacter&, EEnchantmentCategory);
};

}
