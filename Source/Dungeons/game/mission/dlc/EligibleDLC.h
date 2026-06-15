#pragma once
#include "game/dlc/DLCName.h"
#include "EligibleDLC.generated.h"

struct FInventoryItemData;
class UInventoryItem;

USTRUCT(BlueprintType)
struct DUNGEONS_API FEligibleDLC {
	GENERATED_BODY()

private:
	UPROPERTY()
	TArray<EDLCName> eligibleDlc;

	void Append(const FInventoryItemData&);

public:
	FEligibleDLC();
	FEligibleDLC(const TArray<EDLCName>&, const TArray<FInventoryItemData>&);
	FEligibleDLC(const TArray<EDLCName>&, const TArray<UInventoryItem*>&);
	bool IsEligible(EDLCName) const;
};