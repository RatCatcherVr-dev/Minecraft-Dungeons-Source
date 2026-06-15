#include "Dungeons.h"
#include "MissionData.h"
#include "game/GameSettings.h"
#include "game/mission/difficulty/MissionDifficulty.h"
#include "game/item/generator/ItemGenerator.h"
#include "util/Algo.h"
#include "util/Random.h"
#include "util/FloatWeighedRandom.h"
#include "game/item/generator/ItemGeneratorConfigs.h"

//
// Helpers for creating FRareItemChances for given minimum rarity levels
//
namespace {

const FRareItemChance& defaultRarityChancesForMinimumRequiredRarity(EItemRarity minRarity) {
	static const FItemRarityChance common{ EItemRarity::Common, 100.0f }, rare{ EItemRarity::Rare, 10.0f }, unique{ EItemRarity::Unique, 1.0f };
	static const std::array<FRareItemChance, 3> chances {
		FRareItemChance({common, rare, unique}), // min: common
		FRareItemChance({rare, unique}),         // min: rare
		FRareItemChance({unique}),               // min: unique
	};
	return chances[enum_cast(minRarity)];
}

FRareItemChance withMinimumRequiredRarity(const FRareItemChance& chance, EItemRarity minRarity) {
	auto filteredRarities = algo::copy_if(chance.RarityChances,
		RETLAMBDA(it.getWeight() > 0 && static_cast<int>(it.Rarity) >= static_cast<int>(minRarity)));
	return FRareItemChance(!filteredRarities.empty() ? std::move(filteredRarities) : defaultRarityChancesForMinimumRequiredRarity(minRarity).RarityChances);
}

FRareItemChance withMinimumRequiredRarity(EItemRarity minRarity) {
	return withMinimumRequiredRarity(FRareItemChance::GetChanceFromCategory(EItemRarityChanceCategory::MissionEnd), minRarity);
}

}

//
// PredRarity factories for the (F)MissionItem* structs
//
PredRarity createPredRarity(const FMissionItemTagChance& item) {
	return { itemgen::predicates::Tag(item.itemTag) && itemgen::predicates::AllowedOnAnyUnlockedMissionDifficulty() , withMinimumRequiredRarity(item.guaranteedRarity) };
}

FMissionItemTypeChance::FMissionItemTypeChance(const FItemId& type, EItemRarity rarity):ItemId(type), guaranteedRarity(GetItemRegistry().Get(type).getFixedRarity().Get(rarity)) {
}

FMissionItemTypeChance::FMissionItemTypeChance() : ItemId(GetItemRegistry().GetValues()[0]->getId()), guaranteedRarity(EItemRarity::Common) {
}

PredRarity createPredRarity(const FMissionItemTypeChance& item) {
	return { itemgen::predicates::Only(item.ItemId), withMinimumRequiredRarity(item.guaranteedRarity) };
}

PredRarity createPredRarity(const FMissionItemRarityChance& item) {
	return { itemgen::predicates::AllowedOnAnyUnlockedMissionDifficulty() && itemgen::predicates::Item(), withMinimumRequiredRarity(item.guaranteedRarity) };
}

PredRarity createPredRarity(const FMissionItemChance& item) {
	if (item.customConfig) {
		return { item.customConfig->pred, FRareItemChance::GetChanceFromCategory(EItemRarityChanceCategory::MissionEnd) };
	}
	if (item.typeChance) {
		return createPredRarity(item.typeChance.GetValue());
	}
	if (item.tagChance) {
		return createPredRarity(item.tagChance.GetValue());
	}
	if (item.rarityChance) {
		return createPredRarity(item.rarityChance.GetValue());
	}
	checkNoEntry()
	return {};
}


//
// MissionItemChances - a collection of MissionItemChance objects
//
MissionItemChances::MissionItemChances(TArray<FMissionItemChance> itemChances)
	: mItemChances(std::move(itemChances)) {
}

MissionItemChances::MissionItemChances(itemgen::Config itemTypes)
	: mItemChances({ { {}, {}, {}, std::move(itemTypes) } }) {
}

const TArray<FMissionItemChance>& MissionItemChances::itemChances() const {
	return mItemChances;
}

TArray<FMissionItemTypeChance> MissionItemChances::getPossibleItemRarityTypes(const FMissionDifficulty& missionDifficulty) const {
	const game::Settings settings(missionDifficulty, 0, missionDifficulty.mission);
	TSet<TPair<FItemId, EItemRarity>> out;

	for (auto& itemChance : mItemChances) {
		if (itemChance.typeChance) {
			out.Add(TPair<FItemId, EItemRarity> { itemChance.typeChance->ItemId, itemChance.typeChance->guaranteedRarity });
		}
		if (itemChance.customConfig) {
			for (auto& type : getPossibleItemTypeIds(settings, itemgen::looters::Unknown(), itemChance.customConfig.GetValue())) {
				out.Add(TPair<FItemId, EItemRarity> { type, EItemRarity::Common });
			}
		}
	}
	return algo::map_tarray(out, RETLAMBDA((FMissionItemTypeChance{ it.Key, it.Value })));
}
