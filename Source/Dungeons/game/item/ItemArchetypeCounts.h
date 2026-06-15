#pragma once

#include "ItemArchetype.h"
#include <unordered_map>

class UInventoryItem;
struct FInventoryItemData;

struct ItemArchetypeCounts {
	ItemArchetypeCounts() = default;
	ItemArchetypeCounts(std::unordered_map<EItemArchetype, int> counts);
	ItemArchetypeCounts(const TArray<FInventoryItemData>&);
	ItemArchetypeCounts(const TArray<UInventoryItem*>&);

	std::unordered_map<EItemArchetype, int> counts;

	int  get(EItemArchetype) const;

	bool isLargerThan(const ItemArchetypeCounts&) const;	
};
