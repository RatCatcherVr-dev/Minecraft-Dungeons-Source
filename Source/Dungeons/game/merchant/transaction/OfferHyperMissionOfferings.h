#pragma once
#include "game/mission/offerings/MissionOfferings.h"
#include "MissionOfferingsTransactionBase.h"
#include "OfferHyperMissionOfferings.generated.h"

class UInventoryItem;

UCLASS()
class DUNGEONS_API UOfferHyperMissionOfferings : public UMissionOfferingsTransactionBase {
	GENERATED_BODY()
	
protected:	
	void PopulateRequiredSelectionClasses(TSet<TSubclassOf<UMerchantSelectionBase>>& selections) const override;

	FMissionOfferings CreateMissionOfferings() const override;
	bool IsItemBeingOffered(UInventoryItem*) const override;
	FMerchantTransactionStatus Validate() const override;
	EMerchantTransactionStatusReason SuccessReason() const override;
};
