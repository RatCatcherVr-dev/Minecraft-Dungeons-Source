#include "Dungeons.h"
#include "MissionOfferings.h"
#include "game/inventory/InventoryItem.h"
#include "util/Algo.h"

int FMissionOfferings::getOfferedItemCount() const{
	return offeredItems.Num();
}

int FMissionOfferings::getOfferedDisplayItemPower() const {
	return algo::sum(offeredItems, RETLAMBDA(it->GetDisplayItemPowerInt()));
}

FOfferingsSummary FMissionOfferings::getOfferingsSummary() const {
	return { getOfferedItemCount(), getOfferedDisplayItemPower(), offeredEnchantmentPoints };
}

bool FMissionOfferings::operator==(const FMissionOfferings& other) const {
	return offeredEnchantmentPoints == other.offeredEnchantmentPoints && offeredItems == other.offeredItems;
}

