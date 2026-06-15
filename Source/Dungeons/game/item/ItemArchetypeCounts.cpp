#include "Dungeons.h"
#include "ItemArchetypeCounts.h"
#include "InventoryItemData.h"
#include "game/inventory/InventoryItem.h"
#include "util/Algo.h"


namespace internal {
void addFrom(ItemArchetypeCounts& archetypeCounts, const FInventoryItemData& item) {
	for (EItemArchetype archetype : item.GetItemType().getArchetypes()) {
		archetypeCounts.counts[archetype]++;
	}
}
}

ItemArchetypeCounts::ItemArchetypeCounts(const TArray<FInventoryItemData>& items) {
	algo::for_each(items, RETLAMBDA(internal::addFrom(*this, it)));
}

ItemArchetypeCounts::ItemArchetypeCounts(const TArray<UInventoryItem*>& items) {
	algo::for_each(items, RETLAMBDA(internal::addFrom(*this, it->Item)));
}

ItemArchetypeCounts::ItemArchetypeCounts(std::unordered_map<EItemArchetype, int> counts)
	: counts(counts) {
}

int ItemArchetypeCounts::get(EItemArchetype type) const {
	const auto it = counts.find(type);
	return it != counts.end() ? it->second : 0;
}

bool ItemArchetypeCounts::isLargerThan(const ItemArchetypeCounts& other) const {
	return algo::all_of(other.counts, RETLAMBDA(get(it.first) >= it.second));
}
