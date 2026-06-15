#pragma once
#include "InventoryItemMerchantSlotBase.h"
#include "OfferedItemSlot.generated.h"


UCLASS(BlueprintType)
class DUNGEONS_API UOfferedItemSlot : public UInventoryItemMerchantSlotBase {
	GENERATED_BODY()
public:
	TArray<TSubclassOf<UMerchantTransactionBase>> GetAvailableTransactions() const override;
};
