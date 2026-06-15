#include "Dungeons.h"
#include "game/item/InventoryItemData.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/merchant/quest/MerchantQuestBase.h"
#include "game/merchant/type/MerchantBase.h"
#include "ProgressPowerRarityPricing.h"

UProgressPowerRarityPricing* UProgressPowerRarityPricing::CreateSubobject(AMerchantBase* object, const FName& name, int basePrice) {
	auto pricing = object->CreateDefaultSubobject<UProgressPowerRarityPricing>(name);
	pricing->mBasePrice = basePrice;
	return pricing;
}

UProgressPowerRarityPricing* UProgressPowerRarityPricing::SetRestockPricingQuest(int restockBasePrice, UMerchantQuestBase* restockQuest) {
	mRestockBasePrice = restockBasePrice;
	mRestockPriceQuest = restockQuest;
	return this;
}

float UProgressPowerRarityPricing::GetItemRarityMultiplier(const FInventoryItemData& item) const {
	if (item.GetItemType().usesRarity()) {
		switch (item.Rarity) {
		case EItemRarity::Unique:
			return 2.0f;
		case EItemRarity::Rare:
			return 1.4f;
		case EItemRarity::Common:
		default:
			return 1.0f;
		}
	}
	return 1.0f;
}

float UProgressPowerRarityPricing::GetItemSlotTypeMultiplier(const FInventoryItemData& item) const {
	switch (item.GetItemType().getSlotType()) {
	case ESlotType::ActivePermanent:
		return 2.0f;
	default:
		return 1.0f;
	}
}

float UProgressPowerRarityPricing::GetItemGildedMultiplier(const FInventoryItemData& item) const {
	if (item.IsNetherite()) {
		return 3.0f;
	}
	return 1.0f;
}

float UProgressPowerRarityPricing::GetItemPowerMultiplier(const FInventoryItemData& item) const {
	if (item.GetItemType().usesItemPower()) {
		return game::item::power::ShopPriceMultiplier(item.ItemPower);
	} else {
		return GetSession().GetGameDifficultyStats().GetShopPriceMultiplier();
	}
}


float UProgressPowerRarityPricing::GetRestockPriceExponent() const {
	if (mRestockPriceQuest) {
		//A sane limit which guards us from overflowing.
		return FMath::Clamp(static_cast<float>(mRestockPriceQuest->GetCompletedCount()), 0.0f, 16.0f);
	}
	return 0.0f;
}

float UProgressPowerRarityPricing::GetDifficultyMultiplier(const game::DifficultyStats& stats) const {
	return stats.GetShopPriceMultiplier();
}


static int SnapCost(int cost, int snap) {
	return FMath::FloorToInt(static_cast<float>(cost) / static_cast<float>(snap)) * snap;
}

int UProgressPowerRarityPricing::ShopifyCost(int cost) const {
	if (cost >= 50000) {
		return SnapCost(cost, 1000);
	} else if (cost >= 20000) {
		return SnapCost(cost, 500);
	}  else if (cost >= 10000) {
		return SnapCost(cost, 250);
	} else if (cost >= 4000) {
		return SnapCost(cost, 100);
	} else if (cost >= 2000) {
		return SnapCost(cost, 50);
	} else if (cost >= 750) {		
		return SnapCost(cost, 25);
	} else if (cost >= 250) {
		return SnapCost(cost, 10);
	} else if (cost >= 50) {		
		return SnapCost(cost, 5);
	} else {
		return cost;
	}

}
int UProgressPowerRarityPricing::ShopifyCostRebated(int cost) const {
	auto newCost = ShopifyCost(cost);
	if (newCost >= 10 && (newCost % 10 == 0)) {
		return newCost - 1;
	}
	return newCost;
}

TOptional<FMerchantPricing> UProgressPowerRarityPricing::GetPrice(const FInventoryItemData& item, float PriceMultiplier, float RebateFraction) const {
	const float rarityMultiplier = GetItemRarityMultiplier(item);
	const float powerMultiplier = GetItemPowerMultiplier(item);
	const float typeMultiplier = GetItemSlotTypeMultiplier(item);
	const float gildedMultiplier = GetItemGildedMultiplier(item);
	const float calculatedPrice = mBasePrice * powerMultiplier * rarityMultiplier * typeMultiplier * gildedMultiplier * PriceMultiplier * (1.0f - RebateFraction);
	if(RebateFraction > 0.0f){
		return { {ShopifyCostRebated(calculatedPrice), RebateFraction} };
	} else {
		return { {ShopifyCost(calculatedPrice), 0.0f} };
	}
}

TOptional<FMerchantPricing> UProgressPowerRarityPricing::GetGiftPrice(const FInventoryItemData& giftItem, float PriceMultiplier) const {
	const int priceSnap = mBasePrice / 2;
	return { {SnapCost(mBasePrice * game::item::power::ShopPriceMultiplier(giftItem.ItemPower) * PriceMultiplier, priceSnap), 0.0f} };
}

/*
Price for upgrade intent:
* Be bigger for large upgrade jumps
* Be bigger for higher tier items
* Still be significant even if change is rather small for high levels items.
* Low power items should be a bit cheaper even with big delta
* Be more expensive in endless struggle
*/
const float UProgressPowerRarityPricing::moreExpensivePowerThreshold = game::DifficultyStats(game::FDifficulty(EGameDifficulty::Difficulty_3, EThreatLevel::Threat_7)).GetCombinedItemPowerRange().max();
TOptional<FMerchantPricing> UProgressPowerRarityPricing::GetUpgradePrice(const FInventoryItemData& fromItem, const FInventoryItemData& toItem, float PriceMultiplier) const {
	FInventoryItemData deltaItemCopy = toItem;
	const float deltaItemPower = FMath::Max(0.0f, toItem.ItemPower - fromItem.ItemPower);
	const float deltaProgressFraction = game::item::power::GetItemPowerDeltaProgressFraction(deltaItemPower);	
	const float aboveOneProgressPriceMultiplier = toItem.ItemPower > moreExpensivePowerThreshold ? 3.0f : 1.0f;
	auto fromPrice = GetPrice(fromItem, PriceMultiplier, 0.0f);
	auto targetPrice = GetPrice(toItem, deltaProgressFraction * PriceMultiplier, 0.0f);
	if (fromPrice.IsSet() && targetPrice.IsSet()) {
		const float calculatedPrice = fromPrice.GetValue().Price*0.25f + targetPrice.GetValue().Price*0.75f;
		const float scaledCombinedPrice = calculatedPrice * aboveOneProgressPriceMultiplier;
		return { {ShopifyCost(scaledCombinedPrice), 0.0f} };
	}
	return {};
}

TOptional<FMerchantPricing> UProgressPowerRarityPricing::GetRestockPrice() const {
	if (mRestockBasePrice.IsSet()) {
		return { {ShopifyCost(mRestockBasePrice.GetValue()* GetDifficultyMultiplier(GetSession().GetGameDifficultyStats()) * FMath::Pow(2, GetRestockPriceExponent())), 0.0f} };
	}
	return {};
}

void UProgressPowerRarityPricing::OnRestockQuestCompleted() const {
	Super::OnRestockQuestCompleted();
	if (mRestockPriceQuest) {
		mRestockPriceQuest->ResetProgress();
	}
}

void UProgressPowerRarityPricing::EnsureSaveData() const {
	Super::EnsureSaveData();
	if (mRestockPriceQuest) {
		mRestockPriceQuest->EnsureSaveData();
	}
}


