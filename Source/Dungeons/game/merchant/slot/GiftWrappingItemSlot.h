#pragma once
#include "MerchantItemSlotBase.h"
#include "GiftWrappingItemSlot.generated.h"

UCLASS()
class DUNGEONS_API UGiftWrappingItemSlot : public UMerchantItemSlotBase {
	GENERATED_BODY()
public:
	TArray<TSubclassOf<UMerchantTransactionBase>> GetAvailableTransactions() const override;
};