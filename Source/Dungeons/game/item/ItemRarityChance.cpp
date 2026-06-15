#include "Dungeons.h"
#include "ItemRarityChance.h"
#include "util/SharedRandom.h"
#include "util/FloatWeighedRandom.h"
#include "GameplayPrediction.h"
#include "GameFramework/Actor.h"
#include "game/component/EnchantmentComponent.h"
#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "../util/DungeonsGearUtilLibrary.h"

FRareItemChance::FRareItemChance(std::vector<FItemRarityChance> rarityChances)
	: RarityChances(std::move(rarityChances)) {
}

FRareItemChance::FRareItemChance()
	: FRareItemChance(std::vector<FItemRarityChance>{}) {
}

FRareItemChance FRareItemChance::GetChanceFromCategory(EItemRarityChanceCategory category) {
	switch (category) {
	case EItemRarityChanceCategory::MissionEnd:
	case EItemRarityChanceCategory::ProgressCommand:
	case EItemRarityChanceCategory::FancyChest:
	case EItemRarityChanceCategory::LobbyChest:
	case EItemRarityChanceCategory::PiggyBank:
	case EItemRarityChanceCategory::EnchantedMobGroup:
	case EItemRarityChanceCategory::MysteryBox:
		return FRareItemChance({
			{EItemRarity::Common, 75.0f},
			{EItemRarity::Rare, 20.0f},
			{EItemRarity::Unique, 5.0f}
			});
	case EItemRarityChanceCategory::EventMob:
		return FRareItemChance({
			{EItemRarity::Common, 65.0f},
			{EItemRarity::Rare, 25.0f},
			{EItemRarity::Unique, 10.0f}
		});
	case EItemRarityChanceCategory::AncientMob:
		return FRareItemChance({
			{EItemRarity::Common, 0.0f},
			{EItemRarity::Rare, 75.0f},
			{EItemRarity::Unique, 25.0f}
		});
	case EItemRarityChanceCategory::Boss:
		return FRareItemChance({
			{EItemRarity::Common, 0.0f},
			{EItemRarity::Rare, 50.0f},
			{EItemRarity::Unique, 50.0f}
			});
	case EItemRarityChanceCategory::DeluxeChest:
		return FRareItemChance({
			{EItemRarity::Common, 0.0f},
			{EItemRarity::Rare, 75.0f},
			{EItemRarity::Unique, 25.0f}
		});
	case EItemRarityChanceCategory::Mob:
	case EItemRarityChanceCategory::WoodenChest:
	case EItemRarityChanceCategory::Default:
		return FRareItemChance({
			{EItemRarity::Common, 84.0f},
			{EItemRarity::Rare, 15.0f},
			{EItemRarity::Unique, 1.0f}
		});
	case EItemRarityChanceCategory::LuxuryMerchant:
		return FRareItemChance({
			{EItemRarity::Common, 0.0f},
			{EItemRarity::Rare, 80.0f},
			{EItemRarity::Unique, 20.0f}
		});
	case EItemRarityChanceCategory::PiglinMerchant:
		return FRareItemChance({
			{EItemRarity::Common, 50.0f},
			{EItemRarity::Rare, 45.0f},
			{EItemRarity::Unique, 5.0f}
		});
	case EItemRarityChanceCategory::VillageMerchant:
		return FRareItemChance({
			{EItemRarity::Common, 85.0f},
			{EItemRarity::Rare, 15.0f},
			{EItemRarity::Unique, 0.0f},
		});
	case EItemRarityChanceCategory::OnlyCommon:
		return FRareItemChance({
			{EItemRarity::Common, 100.0f},
			{EItemRarity::Rare, 0.0f},
			{EItemRarity::Unique, 0.0f},
		});
	default:
		ensure(false);
		return {};
	}
}

FRareItemChance FRareItemChance::GetAdjustedChanceFromCategory(EItemRarityChanceCategory category, AActor* targetPlayer)
{
	float rareToUniqueConversionFraction = 0;

	FRareItemChance rarity = GetChanceFromCategory(category);
	if (targetPlayer)
	{
		for (FItemRarityChance itemChance : rarity.RarityChances)
		{
			DungeonsGearUtilLibrary::ChangeItemRarity(Cast<ABaseCharacter>(targetPlayer), itemChance.Rarity);
		}
		
	}

	return rarity;
}

void FRareItemChance::ModifyRarityWeights(float rareToUniqueFraction)
{
	float WeightToAdd = 0;
	for (FItemRarityChance& rarityChance : RarityChances)
	{
		switch (rarityChance.Rarity)
		{
		default:
			break;
		case EItemRarity::Rare:
		{
			float RarityChangeWeightAdjusted = rarityChance.Weight * rareToUniqueFraction;
			WeightToAdd += RarityChangeWeightAdjusted;
			rarityChance.Weight -= RarityChangeWeightAdjusted;
		}
		break;
		}
	}

	std::vector<FItemRarityChance>::iterator itr = find_if(RarityChances.begin(), RarityChances.end(), RETLAMBDA(it.Rarity == EItemRarity::Unique));
	if (itr != RarityChances.end())
	{
		itr->Weight += WeightToAdd;
	}
	else
	{
		FItemRarityChance UniqueItemChance;
		UniqueItemChance.Rarity = EItemRarity::Unique;
		UniqueItemChance.Weight = WeightToAdd;
		RarityChances.push_back(UniqueItemChance);
	}
}

EItemRarity FRareItemChance::GetRandomizedItemRarity(int boost) const {
	auto attempts = boost + 1;
	auto itemRarity = EItemRarity::Common;

	while (attempts > 0 && itemRarity == EItemRarity::Common) {
		const auto randomRarityIndex = FloatWeighedRandom::getRandomItemIndex(&Util::sharedRandom(), RarityChances);
		if (randomRarityIndex >= 0) {		
			itemRarity = RarityChances[randomRarityIndex].Rarity;			
		}
		attempts--;
	}

	return itemRarity;
}

TSet<EItemRarity> FRareItemChance::GetPossibleRarities() const {
	TSet<EItemRarity> set;
	for (auto& chance : RarityChances) {
		set.Add(chance.Rarity);
	}
	return set;
}
