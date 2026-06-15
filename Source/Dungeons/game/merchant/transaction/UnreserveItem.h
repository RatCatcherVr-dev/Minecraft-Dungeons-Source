#pragma once
#include "MerchantSlotTransactionBase.h"
#include "UnreserveItem.generated.h"

UCLASS()
class DUNGEONS_API UUnreserveItem : public UMerchantSlotTransactionBase {
	GENERATED_BODY()

protected:
	FMerchantTransactionStatus Validate() const override;
	EMerchantTransactionStatusReason SuccessReason() const override;
};