#include "Dungeons.h"
#include "MissionOfferingsTransactionBase.h"

FMissionOfferings UMissionOfferingsTransactionBase::GetMissionOfferings() const {
	return CreateMissionOfferings();
}

bool UMissionOfferingsTransactionBase::GetIsItemBeingOffered(UInventoryItem* item) const {
	return IsItemBeingOffered(item);
}
