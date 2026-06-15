#pragma once
#include "SelectedInventoryItemTransactionBase.h"
#include "WithdrawItem.generated.h"

UCLASS()
class DUNGEONS_API UWithdrawItem : public USelectedInventoryItemTransactionBase {
	GENERATED_BODY()
protected:
	FMerchantTransactionStatus Validate() const override;
	EMerchantTransactionStatusReason SuccessReason() const override;
};