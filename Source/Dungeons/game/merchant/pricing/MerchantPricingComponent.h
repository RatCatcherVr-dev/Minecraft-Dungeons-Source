#pragma once
#include "game/merchant/MerchantContext.h"
#include <Components/ActorComponent.h>
#include "game/merchant/currency/MerchantPrice.h"
#include "game/merchant/MerchantSubobjectBase.h"
#include "MerchantPricingComponent.generated.h"

UCLASS(Abstract)
class DUNGEONS_API UMerchantPricingComponent : public UMerchantSubobjectBase {
	GENERATED_BODY()	
protected:
	const FMerchantPricingSaveData& ReadSaveData() const;
	FMerchantPricingSaveData& EditSaveData() const;

public:
	virtual TOptional<FMerchantPricing> GetPrice(const FInventoryItemData&, float PriceMultiplier, float RebateFraction) const {
		return {};
	}

	virtual TOptional<FMerchantPricing> GetUpgradePrice(const FInventoryItemData& fromItem, const FInventoryItemData& toItem, float PriceMultiplier) const {
		return {};
	}

	virtual TOptional<FMerchantPricing> GetGiftPrice(const FInventoryItemData& giftItem, float PriceMultiplier) const {
		return {};
	}

	virtual TOptional<FMerchantPricing> GetRestockPrice() const {
		return {};
	}
	
	virtual void OnRestockQuestCompleted() const {};
	virtual void EnsureSaveData() const;
};