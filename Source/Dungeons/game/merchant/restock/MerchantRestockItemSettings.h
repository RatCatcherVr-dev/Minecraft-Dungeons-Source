#pragma once


namespace game {
namespace merchant {
namespace restock {

struct FBargainChance {
	float RebateFraction;
	float Weight;

	float getWeight() const {
		return Weight;
	}
};

struct ItemSettings {
protected:
	game::item::generator::Pred mPred;
	EItemRarityChanceCategory mRarityCategory;
	float mWeight;
	float mPriceMultiplier = 1.0;
	TOptional<TArray<FBargainChance>> mPriceBargains;
	TOptional<float> mPricePowerRollLuckMultiplier;
	game::item::generator::ItemSource mItemSource;

public:
	ItemSettings(const game::item::generator::Pred& Pred, EItemRarityChanceCategory RarityCategory, float weight = 1.0f) : mItemSource(game::item::generator::ItemSource::Merchant) {
		mPred = Pred;
		mRarityCategory = RarityCategory;
		mWeight = weight;
	}

	float getWeight() const {
		return mWeight;
	}

	float GetPriceMultiplier() const {
		return mPriceMultiplier;
	}

	game::item::generator::ItemSource GetItemSource() const {
		return mItemSource;
	}

	const TOptional<TArray<FBargainChance>>& GetPriceBargains() const {
		return mPriceBargains;
	}

	TOptional<float> GetPricePowerRollLuckMultiplier() const {
		return mPricePowerRollLuckMultiplier;
	}

	EItemRarityChanceCategory GetRarityChanceCategory() const {
		return mRarityCategory;
	}

	const game::item::generator::Pred& GetItemGeneratorPredicate() const {
		return mPred;
	}
};



struct MutableItemSettings : public ItemSettings {
	using ItemSettings::ItemSettings;
public:
	MutableItemSettings& priceMultiplier(float multiplier) { mPriceMultiplier = multiplier; return *this; }
	MutableItemSettings& priceBargains(TArray<FBargainChance> bargains) { mPriceBargains = bargains; return *this; }
	MutableItemSettings& pricePowerRollLuckMultiplier(float multiplier) { mPricePowerRollLuckMultiplier = multiplier; return *this; }
	MutableItemSettings& itemSource(itemgen::ItemSource source) { mItemSource = source; return *this; }
	MutableItemSettings& weight(float weight) { mWeight = weight; return *this; }
};

}
}
}