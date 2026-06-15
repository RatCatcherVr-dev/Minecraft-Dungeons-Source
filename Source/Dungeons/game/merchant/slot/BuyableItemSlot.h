#pragma once
#include "MerchantItemSlotBase.h"
#include "BuyableItemSlot.generated.h"

UCLASS()
class DUNGEONS_API UBuyableItemSlot : public UMerchantItemSlotBase {
	GENERATED_BODY()	
public:	
	TArray<TSubclassOf<UMerchantTransactionBase>> GetAvailableTransactions() const override;
	TOptional<FMerchantPricing> GetOptionalPrice() const override;
};