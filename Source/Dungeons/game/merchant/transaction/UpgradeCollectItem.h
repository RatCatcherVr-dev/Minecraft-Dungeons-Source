#pragma once
#include "MerchantSlotTransactionBase.h"
#include "UpgradeCollectItem.generated.h"

UCLASS()
class DUNGEONS_API UUpgradeCollectItem : public UMerchantSlotTransactionBase {
	GENERATED_BODY()

protected:
	FMerchantTransactionStatus Validate() const override;
	EMerchantTransactionStatusReason SuccessReason() const override;
};