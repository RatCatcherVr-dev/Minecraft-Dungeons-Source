#pragma once
#include "MerchantTransactionBase.h"
#include "RestockSlots.generated.h"

UCLASS()
class DUNGEONS_API URestockSlots : public UMerchantTransactionBase {
	GENERATED_BODY()

protected:
	FMerchantTransactionStatus Validate() const override;
	EMerchantTransactionStatusReason SuccessReason() const override;
};