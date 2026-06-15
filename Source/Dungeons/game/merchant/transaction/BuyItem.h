#pragma once
#include "MerchantSlotTransactionBase.h"
#include "BuyItem.generated.h"

UCLASS()
class DUNGEONS_API UBuyItem : public UMerchantSlotTransactionBase {
	GENERATED_BODY()

protected:
	FMerchantTransactionStatus Validate() const override;	
	EMerchantTransactionStatusReason SuccessReason() const override;
};