#pragma once

#include "game/item/InventoryItemData.h"
#include "game/item/ItemRarityChance.h"
#include "game/item/generator/ItemGeneratorConfigs.h"
#include "game/item/ItemType.h"
#include "game/item/drop/ItemDropData.h"
#include <Optional.h>

class APlayerCharacter;


namespace game { namespace item { namespace drop {

using UniqueItemCalculation = std::function<bool()>;
using RarityCalculation = std::function<EItemRarity()>;

using EnchantmentCalcSelection = std::function<itemgen::EnchantmentCalc(bool)>;

RarityCalculation getRarityCalculation(FRareItemChance, UWorld*);

struct DropGenerationInput {
	DropGenerationInput(const AActor* targetPlayer, generator::Pred, FRareItemChance, itemgen::ItemSource);
	DropGenerationInput(EntityType, const AActor* targetPlayer, generator::Pred, FRareItemChance, itemgen::ItemSource);

	TOptional<EntityType> SourceType;
	const APlayerCharacter* TargetPlayer;
	itemgen::ItemSource DropItemSource;
	
	UniqueItemCalculation CalculateUniqueItem;
	RarityCalculation CalculateRarity;
	const generator::Pred Predicate;

	EnchantmentCalcSelection SelectEnchantmentCalc;
	FRareItemChance itemRarityChance;
};

game::Settings getSettings(UWorld* world);
const itemgen::EnchantmentCalc& getEnchantmentCalc(const generator::ItemSource& sourceType, TOptional<EntityType> MobType);
const itemgen::PowerRangeCalc& getPowerRangeCalc(const EItemRarity& rarity, const itemgen::ItemSource& sourceType);
const itemgen::ProbabilityCalc& getProbabilityCalc(const EItemRarity&);

TOptional<itemgen::PowerRange> getPowerRangeMerged(UWorld*, const DropGenerationInput&);
const generator::Pred& Food();
const generator::Pred& Consumable();
const generator::Pred& ConsumableNoWaterBreathing();
const generator::Pred& Arrow();
const generator::Pred& Emerald();
const generator::Pred& Gold();
const generator::Pred& Potion(const TArray<FItemId>& exclude = {});

const generator::Pred& getPredicateForItemCategory(const EDropCategory& dropCategory);

TOptional<FInventoryItemData> generateDroppedItem(UWorld*, const DropGenerationInput&);
TArray<FInventoryItemData> generateDroppedItems(UWorld*, const DropGenerationInput&, int);
TArray<FInventoryItemData> generateDroppedEmeralds(UWorld*, const DropGenerationInput&, int);
TSet<EItemRarity> getPossibleRarities(const DropGenerationInput&);

}}}
