#pragma once
#include "InventoryItemSlotTransactionBase.h"
#include "UpgradeInsertItem.generated.h"

UCLASS()
class DUNGEONS_API UUpgradeInsertItem : public UInventoryItemSlotTransactionBase {
	GENERATED_BODY()		

protected:
	void PopulateRequiredSelectionClasses(TSet<TSubclassOf<UMerchantSelectionBase>>& selections) const;

	FMerchantTransactionStatus Validate() const override;
	EMerchantTransactionStatusReason SuccessReason() const override;
};