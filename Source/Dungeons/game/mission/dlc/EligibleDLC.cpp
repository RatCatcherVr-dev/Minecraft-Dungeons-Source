#include "Dungeons.h"
#include <Anticheat.hpp>
#include "EligibleDLC.h"
#include "game/item/InventoryItemData.h"
#include "game/item/ItemType.h"
#include "game/inventory/InventoryItem.h"
#include "util/Algo.h"

FEligibleDLC::FEligibleDLC(const TArray<EDLCName>& dlc, const TArray<FInventoryItemData>& items) : eligibleDlc(dlc) {
	algo::for_each(items, RETLAMBDA(Append(it)));
}
FEligibleDLC::FEligibleDLC(const TArray<EDLCName>& dlc, const TArray<UInventoryItem*>& items) : eligibleDlc(dlc) {
	algo::for_each(items, RETLAMBDA(Append(it->Item)));
}
FEligibleDLC::FEligibleDLC(){}

ANTICHEAT_NO_OPTIMIZATION_BEGIN
void FEligibleDLC::Append(const FInventoryItemData& item){
	ANTICHEAT_VIRT_BEGIN
	if (auto eligibleDLC = item.GetItemType().getDLCEligibility()) {
		eligibleDlc.AddUnique(eligibleDLC.GetValue());
	}
	ANTICHEAT_VIRT_END
}

bool FEligibleDLC::IsEligible(EDLCName dlc) const {
	ANTICHEAT_VIRT_BEGIN
	return eligibleDlc.Contains(dlc);
	ANTICHEAT_VIRT_END
}

ANTICHEAT_NO_OPTIMIZATION_END
