#pragma once
#include "MerchantSlotTransactionBase.h"
#include "CollectItem.generated.h"

UCLASS()
class DUNGEONS_API UCollectItem : public UMerchantSlotTransactionBase {
	GENERATED_BODY()

protected:
	FMerchantTransactionStatus Validate() const override;
	EMerchantTransactionStatusReason SuccessReason() const override;
};