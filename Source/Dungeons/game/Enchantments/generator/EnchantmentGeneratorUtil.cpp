#include "Dungeons.h"
#include "EnchantmentGeneratorUtil.h"
#include "game/Enchantments/EnchantmentUtil.h"
#include "game/item/ItemType.h"
#include "util/Algo.h"
#include "util/RandomUtil.h"
#include "util/CollectionUtils.h"

namespace game { namespace enchantment { namespace generator {

Enchantments PredState::sameRow() const {
	return row(pt.Y);
}

Enchantments PredState::row(int i) const {
	return Util::subsequence(elements, Counts::Columns * i, Counts::Columns * (i + 1));
}

const EnchantmentType* getRandomEnchantment(const Enchantments& available, PredState state, const std::vector<Pred>& preds, Random* rnd /*= nullptr*/) {
	for (int i : Util::randomIndices(available.Num(), rnd)) {
		const auto enchantment = available[i];
		if (algo::all_of(preds, [&](const Pred& pred) { return pred(state, *enchantment); })) {
			return enchantment;
		}
	}
	return nullptr;
}

}}}
