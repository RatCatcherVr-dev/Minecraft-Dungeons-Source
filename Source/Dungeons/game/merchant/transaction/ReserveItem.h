#pragma once
#include "MerchantSlotTransactionBase.h"
#include "ReserveItem.generated.h"

UCLASS()
class DUNGEONS_API UReserveItem : public UMerchantSlotTransactionBase {
	GENERATED_BODY()

protected:
	FMerchantTransactionStatus Validate() const override;
	EMerchantTransactionStatusReason SuccessReason() const override;
};