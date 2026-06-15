#pragma once
#include "InventoryItemSlotTransactionBase.h"
#include "GiftWrapItem.generated.h"

class APlayerCharacter;

UCLASS()
class DUNGEONS_API UGiftWrapItem : public UInventoryItemSlotTransactionBase {
	GENERATED_BODY()

protected:	
	void PopulateRequiredSelectionClasses(TSet<TSubclassOf<UMerchantSelectionBase>>& selections) const override;
	FMerchantTransactionStatus Validate() const override;	
	EMerchantTransactionStatusReason SuccessReason() const override;

public:
	APlayerCharacter* GetSelectedRecipient() const;

	//This is somewhat ugly, but i would like to contain the logic in the same file as the rest of the transaction
	static bool rerollItemPower_LocalOnly(AActor* recipient, FInventoryItemData& item);
};
