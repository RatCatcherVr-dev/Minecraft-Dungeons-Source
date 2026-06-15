#pragma once
#include "MerchantPricingComponent.h"
#include "game/merchant/MerchantContext.h"
#include "ProgressPowerRarityPricing.generated.h"

class UMerchantQuestBase;
class AMerchantBase;

UCLASS()
class DUNGEONS_API UProgressPowerRarityPricing : public UMerchantPricingComponent {
	GENERATED_BODY()
private:
	int mBasePrice = 1;
	TOptional<int> mRestockBasePrice;
	
	UPROPERTY()
	UMerchantQuestBase* mRestockPriceQuest = nullptr;

	float GetItemRarityMultiplier(const FInventoryItemData&) const;
	float GetItemPowerMultiplier(const FInventoryItemData&) const;
	float GetItemSlotTypeMultiplier(const FInventoryItemData& item) const;
	float GetItemGildedMultiplier(const FInventoryItemData& item) const;
	float GetRestockPriceExponent() const;
	float GetDifficultyMultiplier(const game::DifficultyStats&) const;

	int ShopifyCost(int cost) const;
	int ShopifyCostRebated(int cost) const;

	static const float moreExpensivePowerThreshold;

public:
	static UProgressPowerRarityPricing* CreateSubobject(AMerchantBase* object, const FName& name, int basePrice);
	UProgressPowerRarityPricing* SetRestockPricingQuest(int restockBasePrice, UMerchantQuestBase* restockQuest);

	TOptional<FMerchantPricing> GetPrice(const FInventoryItemData& item, float PriceMultiplier, float RebateFraction) const override;
	TOptional<FMerchantPricing> GetGiftPrice(const FInventoryItemData& giftItem, float PriceMultiplier) const override;
	TOptional<FMerchantPricing> GetUpgradePrice(const FInventoryItemData& fromItem, const FInventoryItemData& toItem, float PriceMultiplier) const;
	TOptional<FMerchantPricing> GetRestockPrice() const override;

	void OnRestockQuestCompleted() const override;
	void EnsureSaveData() const override;
};