#include "Dungeons.h"
#include "game/item/generator/ItemGenerator.h"
#include "game/item/drop/ItemDropGenerator.h"
#include "game/merchant/type/MerchantBase.h"
#include "MerchantPredicateRarityGenerator.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "util/FloatWeighedRandom.h"

UMerchantPredicateRarityGenerator* UMerchantPredicateRarityGenerator::CreateSubobject(AMerchantBase* merchant, const FName& name, const game::item::generator::Pred& itemGeneratorPredicate, EItemRarityChanceCategory rarityCategory) {
	return CreateSubobject(merchant, name, { {itemGeneratorPredicate, rarityCategory, 1.0f} });
}

UMerchantPredicateRarityGenerator* UMerchantPredicateRarityGenerator::CreateSubobject(AMerchantBase* merchant, const FName& name, const TArray<game::merchant::restock::ItemSettings>& weightedDrops) {
	auto gen = merchant->CreateDefaultSubobject<UMerchantPredicateRarityGenerator>(name);
	gen->mWeightedDropSettings = weightedDrops;
	return gen;
}

TOptional<FMerchantItemData> UMerchantPredicateRarityGenerator::generate() const {
	const auto dropPredIndex = FloatWeighedRandom::getRandomItemIndex(&Util::sharedRandom(), mWeightedDropSettings);
	if (dropPredIndex >= 0) {
		auto& dropSettings = mWeightedDropSettings[dropPredIndex];
		const game::item::drop::DropGenerationInput dropData(GetContext().GetMerchant().GetPlayerCharacterOwner(), dropSettings.GetItemGeneratorPredicate(), FRareItemChance::GetChanceFromCategory(dropSettings.GetRarityChanceCategory()), dropSettings.GetItemSource());
		auto maybeItem = game::item::drop::generateDroppedItem(GetSession().GetShopper().GetWorld(), dropData);
		if (maybeItem.IsSet()) {
			float priceMultiplier = dropSettings.GetPriceMultiplier();
			float rebateFraction = 0.0f;
			if (auto bargains = dropSettings.GetPriceBargains()) {
				const int bargainIndex = FloatWeighedRandom::getRandomItemIndex(&Util::sharedRandom(), bargains.GetValue());
				auto& bargain = bargains.GetValue()[bargainIndex];
				rebateFraction = bargain.RebateFraction;
			}
			if (auto powerRollLuckMultiplier = dropSettings.GetPricePowerRollLuckMultiplier()) {
				auto referenceStats = GetSession().GetDifficultyRecommendation().getDifficultyStats(EExtraChallenge::NoExtraChallenge);
				const float rolledItemPowerFraction = FMath::Max(0.0f, referenceStats.GetCombinedItemPowerRange().fractionAt(maybeItem.GetValue().ItemPower));
				priceMultiplier = priceMultiplier + priceMultiplier * powerRollLuckMultiplier.GetValue() * rolledItemPowerFraction;
			}
			return { { maybeItem.GetValue(), priceMultiplier, rebateFraction } };
		}
	}
	return {};
}
