#pragma once
#include "OfferingsSummary.h"
#include "MissionOfferings.generated.h"

class UInventoryItem;

USTRUCT(BlueprintType)
struct DUNGEONS_API FMissionOfferings {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TArray<UInventoryItem*> offeredItems;
	
	UPROPERTY(BlueprintReadOnly)
	int offeredEnchantmentPoints = 0;

	int getOfferedItemCount() const;
	int getOfferedDisplayItemPower() const;

	FOfferingsSummary getOfferingsSummary() const;

	bool operator==(const FMissionOfferings& other) const;
};
