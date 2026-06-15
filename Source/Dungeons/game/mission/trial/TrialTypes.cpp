#include "Dungeons.h"
#include "TrialTypes.h"
#include "game/affector/AffectorTypes.h"
#include "game/LevelSettings.h"
#include "util/Algo.h"

FTrialDef FReplicatableTrialDef::ToTrialDef() const {
	FTrialDef fullDef;
	fullDef.level = level;
	fullDef.extraChallenge = extraChallenge;
	fullDef.affectors = ParseAffectors();
	fullDef.rewards = ParseRewards();
	fullDef.id = id;
	fullDef.type = type;
	if (Theme != EMissionTheme::Invalid) fullDef.theme = Theme;
	return fullDef;
}

affector::RuleData FReplicatableTrialDef::ParseAffectors() const {
	affector::RuleData affectors;
	for (const auto& modifier : modifiers.pairs) {
		const auto affectorId = affector::idFromString(modifier.key);
		if (affectorId != EAffectorId::Last) {
			affectors.Set(affectorId, FAffectorData(modifier.value));
		}
	}
	return affectors;
}

TArray<FMissionItemChance> FReplicatableTrialDef::ParseRewards() const {
	TArray<FMissionItemChance> parsedRewards;
	for (const auto& reward : rewards) {
		if (const auto tagChanceReward = ParseTagChanceReward(reward)) {
			parsedRewards.Add(tagChanceReward.GetValue());
		} else if(const auto typeChanceReward = ParseTypeChanceReward(reward)) {			
			parsedRewards.Add(typeChanceReward.GetValue());
		}

		if (parsedRewards.Num() == 0) {
			parsedRewards.Add(ParseRarityChanceReward(reward));
		}
	}
	return parsedRewards;
}

TOptional<FMissionItemChance> FReplicatableTrialDef::ParseTagChanceReward(const FReplicatableMap& reward) {
	if (const auto type = reward.Find("type")) {
		const auto itemTag = game::item::tag::fromString(type.GetValue());
		ensure(itemTag);

		if (itemTag) {
			FMissionItemTagChance tagChance;
			tagChance.itemTag = itemTag.GetValue();
			tagChance.guaranteedRarity = ParseRarity(reward);
			
			auto itemChance = FMissionItemChance();
			itemChance.tagChance = tagChance;
			itemChance.weight = ParseWeight(reward);
			return itemChance;
		}
	}
	return {};
}

TOptional<FMissionItemChance> FReplicatableTrialDef::ParseTypeChanceReward(const FReplicatableMap& reward) {
	if (const auto item = reward.Find("item")) {
		if (const auto itemType = GetItemRegistry().Request(*item.GetValue())) {
			FMissionItemTypeChance typeChance(itemType.GetValue(), ParseRarity(reward));
			
			auto itemChance = FMissionItemChance();
			itemChance.typeChance = typeChance;
			itemChance.weight = ParseWeight(reward);
			return itemChance;	
		}
	}
	return {};
}

FMissionItemChance FReplicatableTrialDef::ParseRarityChanceReward(const FReplicatableMap& reward) {
	FMissionItemRarityChance rarityChance;
	rarityChance.guaranteedRarity = ParseRarity(reward);

	auto itemChance = FMissionItemChance();
	itemChance.rarityChance = rarityChance;
	itemChance.weight = ParseWeight(reward);
	return itemChance;
}

EItemRarity FReplicatableTrialDef::ParseRarity(const FReplicatableMap& reward) {
	if (const auto rarity = reward.Find("rarity")) {
		return game::item::rarity::fromString(rarity.GetValue());
	}
	return EItemRarity::Common;
}

float FReplicatableTrialDef::ParseWeight(const FReplicatableMap& reward) {
	if (const auto weight = reward.Find("weight")) {
		return FCString::Atof(*weight.GetValue());
	}
	return 1.0f;
}

FString FReplicatableTrialDef::ToString() const {
	// TODO: Add more trial data to print-out.
	return FString::Printf(TEXT("Mission: %s"), *GetEnumValueToString(level));
}

//
// FReplicatableTrialDefPack - A collection of (replicable) trial defs
//
void FReplicatableTrialDefPack::Set(const TArray<FReplicatableTrialDef>& trials) {
	trialDefs = trials;
	isSet = true;
}

bool FReplicatableTrialDefPack::IsSet() const {
	return isSet;
}

void FReplicatableTrialDefPack::MarkSet() {
	isSet = true;
}

TArray<FTrialDef> FReplicatableTrialDefPack::ToTrialDefs() const {
	return algo::map_tarray(trialDefs, RETLAMBDA(it.ToTrialDef()));
}
