#pragma once
#include "SelectedInventoryItemTransactionBase.h"
#include "OfferItem.generated.h"

UCLASS()
class DUNGEONS_API UOfferItem : public USelectedInventoryItemTransactionBase {
	GENERATED_BODY()
protected:
	FMerchantTransactionStatus Validate() const override;
	EMerchantTransactionStatusReason SuccessReason() const override;
};