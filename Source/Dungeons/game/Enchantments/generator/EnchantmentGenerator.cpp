#include "Dungeons.h"
#include "EnchantmentGenerator.h"
#include "game/Enchantments/EnchantmentUtil.h"
#include "game/item/ItemType.h"
#include "util/Algo.h"
#include "util/RandomUtil.h"
#include "util/SharedRandom.h"
#include "util/CollectionUtils.h"
#include "EnchantmentGeneratorUtil.h"

namespace game { namespace enchantment { namespace generator {

AvailableEnchantments getAvailableEnchantmentsForType(const FItemId& type) {
	const ItemType& item = GetItemRegistry().Get(type);
	const EEnchantmentCategory enchantmentCategory = enchantment::type::EnchantmentCategoryFromItemTag(item.getTag());
	const TArray<FEnchantmentData> defaultEnchantments = item.getDefaultEnchantments();
	const TArray<FEnchantmentData> blockedEnchantemnts = item.getBlockedEnchantments();

	AvailableEnchantments out;
	for (const auto& enchantment : type::getAvailableEnchantments()) {
		if (enchantment.isWorkInProgress()) {
			continue;
		}

		if (enchantment.isDisabledForPlayers()) {
			continue;
		}
		if (!(enchantment.getEnchantmentCategory() & enchantmentCategory)) {
			continue;
		}
		if (enchantment.hasTag(EEnchantmentTag::SoulUsage) && (!item.isSoulGatherItem() && !item.isSoulGatherBoostingItem())) {
			continue;
		}
		if (enchantment.hasTag(EEnchantmentTag::ChargingWeaponsOnly) && !item.isBow()) {
			continue;
		}
		if (defaultEnchantments.ContainsByPredicate([&](const FEnchantmentData& data) { return data.TypeID == enchantment.getEnchantmentTypeID();  })) {
			//Don't roll enchantments which are already default
			continue;
		}
		if (blockedEnchantemnts.ContainsByPredicate([&](const FEnchantmentData& data) { return data.TypeID == enchantment.getEnchantmentTypeID();  })) {
			//Don't roll enchantments which are blocked on this item
			continue;
		}
		(enchantment.isRarity(EEnchantmentRarity::Powerful) ? out.powerful : out.nonPowerful).Add(&enchantment);
	}
	return out;
}

static const std::vector<Pred> DefaultPredicates{
	[](PredState state, const EnchantmentType& e) { return e.hasTag(EEnchantmentTag::Stacking) ? !state.sameRow().Contains(&e) : !state.elements.Contains(&e); },
	[](PredState state, const EnchantmentType& e) { return !e.hasTag(EEnchantmentTag::Multi) || !state.sameRow().ContainsByPredicate([](auto& o) { return o && o->hasTag(EEnchantmentTag::Multi); }); },
};

std::vector<size_t> generateRowCounts(int rows, int count, int minCountPerRow, Random* rnd) {
	std::vector<size_t> counts(rows, minCountPerRow);
	int remaining = [&]() {
		const int currentCount = rows * minCountPerRow;
		const int maxPossibleCount = rows * Counts::Columns;
		return Math::clamp(count - currentCount, 0, maxPossibleCount - currentCount);
	}();
	while (remaining) {
		size_t& rowCount = *Util::randomChoice(counts, rnd);
		if (rowCount < Counts::Columns) {
			rowCount++;
			remaining--;
		}
	}
	return counts;
}

TArray<int> getRightAlignedIndices(const std::vector<size_t>& rowCounts) {
	TArray<int> indices;
	for (size_t i = 0; i < rowCounts.size(); ++i) {
		for (int j = Counts::Columns - rowCounts[i]; j < Counts::Columns; ++j) {
			indices.Add(i * Counts::Columns + j);
		}
	}
	return indices;
}

TArray<int> getPowerfulIndices(TArray<int> allIndices, const Counts& counts, Random* rnd) {
	// We can only pick from the last config.powerfulRows rows, and first/one slot should be from the last row
	allIndices.RemoveAllSwap([min = counts.firstIndexOfRow(counts.rows - counts.powerfulRows)](int i) { return i < min; });

	if (const int count = std::min(allIndices.Num(), counts.powerfulEnchantments)) {
		algo::random::shuffle(allIndices, *Util::thisOrSharedRandom(rnd));
		allIndices.Swap(0, allIndices.IndexOfByPredicate([min = counts.firstIndexOfRow(counts.rows - 1)](int i) { return i >= min; }));
		return Util::subsequence(allIndices, 0, count);
	}
	return {};
}

TArray<FEnchantmentData> generate(const AvailableEnchantments& available, Counts counts, Random* rnd) {
	if (available.nonPowerful.Num() == 0 && available.powerful.Num() == 0) {
		return {};
	}
	const auto rowCounts = generateRowCounts(counts.rows, counts.enchantments, counts.minCountPerRow, rnd);
	const auto allIndices = getRightAlignedIndices(rowCounts);
	const auto powerfulIndices = getPowerfulIndices(allIndices, counts, rnd);

	auto slots = Util::createZeroedTArrayOfSize<const EnchantmentType*>(counts.rows * Counts::Columns);
	for (auto index : allIndices) {
		const auto  coordinate = FIntPoint{ index % Counts::Columns, index / Counts::Columns };
		const auto& enchantments = powerfulIndices.Contains(index) ? available.powerful : available.nonPowerful;
		slots[index] = getRandomEnchantment(enchantments, { slots, coordinate }, DefaultPredicates, rnd);
	}
	return Util::map(RETLAMBDA(it ? FEnchantmentData(it->getEnchantmentTypeID(), 0) : FEnchantmentData{}), slots);
}

TArray<FEnchantmentData> generate(const FItemId& type, Counts counts, Random* rnd) {
	return generate(getAvailableEnchantmentsForType(type), counts, rnd);
}

}}}
