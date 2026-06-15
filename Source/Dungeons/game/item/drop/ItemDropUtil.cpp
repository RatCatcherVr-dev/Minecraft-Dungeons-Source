#include "Dungeons.h"
#include "ItemDropUtil.h"
#include "ItemDropGenerator.h"

namespace game { namespace item { namespace drop {

bool isGearOrPermanent(EDropGeneratorCategory category) {
	return category == EDropGeneratorCategory::Gear || category == EDropGeneratorCategory::PermanentItem;
}

bool isExchangeableToGold(const FNetworkedItemDropData& dropData) {
	return isGearOrPermanent(dropData.GetDropData().GeneratorCategory);
};

auto getDropCountCalculation(EItemRarityChanceCategory rarityChanceCategory, UWorld* world) {
	return [calculateRarity = getRarityCalculation(FRareItemChance::GetChanceFromCategory(rarityChanceCategory), world)]() {
		switch (calculateRarity()) {
		case EItemRarity::Common: return 1;
		case EItemRarity::Rare: return 2;
		case EItemRarity::Unique: return 3;
		}
		checkNoEntry();
		return 1;
	};
}

TArray<FNetworkedItemDropData> toHyperMissionDrops(const TArray<FNetworkedItemDropData>& drops, EItemRarityChanceCategory rarityChanceCategory, const AActor* sourceActor) {

	static const int32 HYPERMISSION_MIN_DROP_COUNT_MULTIPLIER = 1;
	static const int32 HYPERMISSION_MAX_DROP_COUNT_MULTIPLIER = 2;
	static const float HYPERMISSION_PROBABILITY_MULTIPLIER = 2.0f;

	TArray<FNetworkedItemDropData> result;
	const auto calculateDropCount = getDropCountCalculation(rarityChanceCategory, sourceActor->GetWorld());
	for (const auto& drop : drops) {
		if (isExchangeableToGold(drop)) {
			const int dropCount = calculateDropCount();
			if (auto goldDrop = FNetworkedItemDropData::FromFItemDrop({ EDropCategory::Gold, dropCount * HYPERMISSION_MIN_DROP_COUNT_MULTIPLIER, dropCount * HYPERMISSION_MAX_DROP_COUNT_MULTIPLIER, drop.GetProbability() * HYPERMISSION_PROBABILITY_MULTIPLIER })) {
				result.Add(goldDrop.GetValue());
			}
		}
		else {
			result.Add(drop);
		}
	}
	return result;
}


}}}
