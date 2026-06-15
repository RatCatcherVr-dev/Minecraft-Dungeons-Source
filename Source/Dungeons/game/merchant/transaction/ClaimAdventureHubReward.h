#pragma once
#include "MerchantSlotTransactionBase.h"
#include "ClaimAdventureHubReward.generated.h"

UCLASS()
class DUNGEONS_API UClaimAdventureHubReward : public UMerchantTransactionBase {
	GENERATED_BODY()

protected:
	FMerchantTransactionStatus Validate() const override;
	EMerchantTransactionStatusReason SuccessReason() const override;
};