#pragma once
#include "InventoryItemSlotTransactionBase.h"
#include "ModifyItem.generated.h"

UCLASS()
class DUNGEONS_API UModifyItem : public UInventoryItemSlotTransactionBase {
	GENERATED_BODY()		

protected:
	virtual void PopulateRequiredSelectionClasses(TSet<TSubclassOf<UMerchantSelectionBase>>& selections) const override;
	static TArray<int32> AvailableEnchantmentIndexes(const UInventoryItem*);
	static TOptional<int32> GetRandomSelectedEnchantmentIndex(const UInventoryItem*);
	TOptional<FEnchantmentData> GenerateReplacementEnchantment(UInventoryItem* maybeItem, merchant::Session session) const;

	virtual FMerchantTransactionStatus Validate() const override;
	virtual EMerchantTransactionStatusReason SuccessReason() const override;
};