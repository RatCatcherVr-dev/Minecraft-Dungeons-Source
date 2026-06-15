#include "Dungeons.h"
#include "ItemDropGenerator.h"
#include "game/item/ItemType.h"
#include "game/item/generator/ItemGenerator.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/Game.h"
#include "game/item/generator/ItemGeneratorTypes.h"
#include "game/item/generator/ItemGeneratorLooterState.h"
#include <functional>
#include "game/GameSettings.h"
#include "util/SharedRandom.h"
#include "util/FloatWeighedRandom.h"
#include "game/util/DungeonsGearUtilLibrary.h"
#include "Optional.h"
#include "game/GameBP.h"
#include "game/difficulty/Difficulty.h"
#include "game/util/GearUtil.h"



static int sEmeraldValueMergeStepCount = 5;
FAutoConsoleVariableRef CVarsEmeraldValueMergeStepCount(
	TEXT("Dungeons.Emeralds.ValueMergeStep"),
	sEmeraldValueMergeStepCount,
	TEXT("Number of emerald drops before indevidual actor drops begin increasing in value, (5 would mean 5 val1, then 5 val 2, then 5 val 3 accumulated up to the max value of drops passed in)"),
	ECVF_Default);



namespace game { namespace item { namespace drop {

FItemId getRandomizedWeightedType(const std::vector<FItemDropChance>& dropChances, const FItemId& defaultType) {
	const auto randomRarityIndex = FloatWeighedRandom::getRandomItemIndex(&Util::sharedRandom(), dropChances);

	if (randomRarityIndex >= 0) {
		return dropChances[randomRarityIndex].ItemType;
	}

	return defaultType;
}

const generator::Pred& Emerald() {
	static const generator::Pred pred = game::item::generator::predicates::Only(game::item::type::Emerald.getId());
	return pred;
}

const generator::Pred& Gold() {
	static const generator::Pred pred = game::item::generator::predicates::Only(game::item::type::Gold.getId());
	return pred;
}


static const TArray<FItemId>& foodTypes() {
	static TArray<FItemId> foodTypes = {
		game::item::type::Food1.getId(),
		game::item::type::Food2.getId(),
		game::item::type::Food3.getId(),
		game::item::type::Food4.getId(),
		game::item::type::Food5.getId(),
		game::item::type::Food6.getId(),
		game::item::type::Food7.getId(),
		game::item::type::Food8.getId(),
	};

	return foodTypes;
}

const generator::Pred& Food() {
	static const generator::Pred pred = [](const generator::PredState& state) {		
		if( foodTypes().Contains(state.item.getId()) ) {
			return state.item.isAllowedOnMissionDifficulty(
				state.env.settings.levelName, state.env.settings.difficulty
			);
		}
		return false;
	};

	return pred;
}

static const TArray<FItemId>& consumableTypes() {
	static const TArray<FItemId> consumableTypes = {
		game::item::type::SwiftnessPotion.getId(),
		game::item::type::BackstabbersBrew.getId(),
		game::item::type::StrengthPotion.getId(),
		game::item::type::DefensePotion.getId(), // D11.DB
		game::item::type::IcePotion.getId(), // D11.DB
		game::item::type::DenseBrewPotion.getId(), // D11.CM
		game::item::type::WaterBreathingPotion.getId(), // D11.SSN
		game::item::type::BurningBrewPotion.getId(), // D11.SSN
		game::item::type::TNTBox.getId(),
		game::item::type::Trident.getId()
	};

	return consumableTypes;
}

const generator::Pred& Consumable() {

	static const generator::Pred pred = [](const generator::PredState& state) {		
		if( consumableTypes().Contains( state.item.getId() ) ) {
			return state.item.isAllowedOnMissionDifficulty(
				state.env.settings.levelName, state.env.settings.difficulty
			);
		}
		return consumableTypes().Contains(state.item.getId());
	};

	return pred;
}

const generator::Pred& ConsumableNoWaterBreathing() {
	static const generator::Pred pred = [](const generator::PredState& state) {
		if (consumableTypes().Contains(state.item.getId())) {
			return state.item.isAllowedOnMissionDifficulty(state.env.settings.levelName, state.env.settings.difficulty) && state.item.getId() != game::item::type::WaterBreathingPotion.getId();
		}
		return consumableTypes().Contains(state.item.getId());
	};
	return pred;
}

static const TArray<FItemId>& potionTypes() {
	static const TArray<FItemId> potionTypes = {
		game::item::type::SwiftnessPotion.getId(),
		game::item::type::BackstabbersBrew.getId(),
		game::item::type::StrengthPotion.getId(),
		game::item::type::DefensePotion.getId(), // D11.DB
		game::item::type::IcePotion.getId(), // D11.DB
		game::item::type::DenseBrewPotion.getId(), // D11.CM
		game::item::type::WaterBreathingPotion.getId(), // D11.SSN
		game::item::type::BurningBrewPotion.getId(), // D11.SSN
	};
	return potionTypes;
}

const generator::Pred& Potion(const TArray<FItemId>& exclude) {
	static const generator::Pred pred = [exclude](const generator::PredState& state) {
		if (potionTypes().Contains(state.item.getId()) && !exclude.Contains(state.item.getId())) {
			return state.item.isAllowedOnMissionDifficulty(
				state.env.settings.levelName, state.env.settings.difficulty
			);
		}
		return potionTypes().Contains(state.item.getId());
	};
	return pred;
}

const generator::Pred& Arrow() {
	static const std::vector<FItemDropChance> arrowDropChances = {
		{game::item::type::Arrow.getId(), 10},
	};

	static const generator::Pred pred = [](const generator::PredState& state) {		
		const auto randomRarityIndex = FloatWeighedRandom::getRandomItemIndex(&Util::sharedRandom(), arrowDropChances);

		if (randomRarityIndex >= 0) {
			const auto randomArrow = itemgen::predicates::Only(arrowDropChances[randomRarityIndex].ItemType);
			return randomArrow(state);
		}

		const auto defaultArrow = itemgen::predicates::Only(game::item::type::Arrow.getId());
		return defaultArrow(state);
	};

	return pred;
}

TArray<FItemId> getItemTypesForDropCategory(EDropCategory category, const Settings& settings) {
	TArray<FItemId> itemTypes;

	switch (category)
	{
	case EDropCategory::Food:
		return foodTypes();
		break;
	case EDropCategory::Consumable:
		return consumableTypes();
		break;
	case EDropCategory::ConsumableNoWaterBreathing:
		itemTypes.Append(consumableTypes());
		itemTypes.Remove(game::item::type::WaterBreathingPotion.getId());
		break;
	case EDropCategory::Potion:
		return potionTypes();
		break;
	case EDropCategory::Arrow:
		itemTypes.Add(game::item::type::Arrow.getId());
		break;
	case EDropCategory::Emerald:
		itemTypes.Add(game::item::type::Emerald.getId());
		break;
	case EDropCategory::Gold:
		itemTypes.Add(game::item::type::Gold.getId());
		break;
	case EDropCategory::Gear:
		return item::generator::getPossibleItemTypeIds(settings, itemgen::looters::Unknown(), itemgen::configs::DefaultGear());
		break;
	case EDropCategory::None:
	case EDropCategory::PermanentItem:
		break;
	default:
		break;
	}

	return itemTypes;
}

TArray<FItemId> getItemTypesForAllCategories(const Settings& settings) {
	TArray<FItemId> result(getItemTypesForDropCategory(EDropCategory::Food, settings));

	result.Append(getItemTypesForDropCategory(EDropCategory::Consumable, settings));
	result.Append(getItemTypesForDropCategory(EDropCategory::Arrow, settings));
	result.Append(getItemTypesForDropCategory(EDropCategory::Emerald, settings));
	result.Append(getItemTypesForDropCategory(EDropCategory::Gear, settings));

	return result;
}

const generator::Pred& getPredicateForItemCategory(const EDropCategory& dropCategory) {
	switch (dropCategory) {
		case EDropCategory::Gear:
			return itemgen::predicates::Gear();
		case EDropCategory::PermanentItem:
			return itemgen::predicates::Item();
		case EDropCategory::Emerald:
			return Emerald();
		case EDropCategory::Gold:
			return Gold();
		case EDropCategory::Food:
			return Food();
		case EDropCategory::Consumable:
			return Consumable();
		case EDropCategory::ConsumableNoWaterBreathing:
			return itemgen::predicates::ConsumableNoWaterBreathing();
		case EDropCategory::Arrow:
			return Arrow();
		case EDropCategory::Potion:
			return Potion();
		default:
			return itemgen::predicates::None();
	}
}

RarityCalculation getRarityCalculation(FRareItemChance rareItemChance, UWorld* world) {
	return [rareItemChance = std::move(rareItemChance), world]() {
		int boost = 0;
		if (world) {
			if (auto game = actorquery::getGame(world)) {
				boost = game->settings().difficultyStats.GetLootQualityBoost();
			}
		}
		return rareItemChance.GetRandomizedItemRarity(boost);
	};
}

DropGenerationInput::DropGenerationInput(
		const AActor * targetPlayer, 
		generator::Pred predicate,
		FRareItemChance rareItemChance, 
		itemgen::ItemSource itemSource)
	: TargetPlayer(Cast<APlayerCharacter>(targetPlayer))
	, DropItemSource(itemSource)
	, CalculateRarity(getRarityCalculation(rareItemChance, targetPlayer->GetWorld()))
	, Predicate(predicate)
	, itemRarityChance(rareItemChance)
{
}

DropGenerationInput::DropGenerationInput(
		EntityType type,
		const AActor* targetPlayer,
		generator::Pred predicate,
		FRareItemChance rareItemChance,
		itemgen::ItemSource itemSource)
	: SourceType(type)
	, TargetPlayer(Cast<APlayerCharacter>(targetPlayer))
	, DropItemSource(itemSource)
	, CalculateRarity(getRarityCalculation(rareItemChance, targetPlayer->GetWorld()))
	, Predicate(std::move(predicate))
	, itemRarityChance(rareItemChance) {
}

game::Settings getSettings(UWorld* world) {
	const auto game = actorquery::getGame(world);
	return game ? game->settings() : Settings();
}

int8 calculateRarityIndex(const DropGenerationInput& input) {
	return enum_cast(input.CalculateRarity());
}

EItemRarity asRarity(const uint8& rarityIndex) {
	return static_cast<EItemRarity>(rarityIndex);
}

itemgen::Looter getExistingTargetPlayerLooterState(const APlayerCharacter* character) {
	if (character) {
		return itemgen::looters::FromPlayerCharacter(*character);
	} else {
		return itemgen::looters::Unknown();
	}	
}

const itemgen::EnchantmentCalc& getEnchantmentCalc(const generator::ItemSource& sourceType, TOptional<EntityType> MobType) {
	switch (sourceType) {
		case itemgen::ItemSource::MissionReward:
		case itemgen::ItemSource::Merchant:
		case itemgen::ItemSource::LootUnlocker:
		case itemgen::ItemSource::Drop:
			if (MobType) {
				return itemgen::enchantmentcalcs::DefaultDroppedByMob(MobType.GetValue());
			}
		default:
			return itemgen::enchantmentcalcs::Default();
	}
}
const itemgen::PowerRangeCalc& getPowerRangeCalc(const EItemRarity& rarity, const generator::ItemSource& sourceType) {
	switch(rarity) {
		case EItemRarity::Unique:
			return itemgen::powerrangecalcs::FromSourceUnique(sourceType);
		case EItemRarity::Rare:
			return itemgen::powerrangecalcs::FromSourceRare(sourceType);
		default:
			return itemgen::powerrangecalcs::FromSource(sourceType);
	}
}

const itemgen::ProbabilityCalc& getProbabilityCalc(const EItemRarity& rarity) {
	switch(rarity) {
		case EItemRarity::Unique:
			return itemgen::probabilitycalcs::Unique();
		default:
			return itemgen::probabilitycalcs::NonUnique();
	}
}

itemgen::EnchantmentCalc getNetheriteEnchantmentCalc(const generator::ItemSource& sourceType, TOptional<EntityType> MobType) {
	switch (sourceType) {
	case itemgen::ItemSource::NetheriteMerchant:
		return itemgen::enchantmentcalcs::NetheriteMerchant();
	case itemgen::ItemSource::Merchant:
	case itemgen::ItemSource::MissionReward:
	case itemgen::ItemSource::LootUnlocker:
	case itemgen::ItemSource::Drop:
		if (MobType) {
			return itemgen::enchantmentcalcs::NetheriteDroppedByMob(MobType.GetValue());
		}
	default:
		return itemgen::enchantmentcalcs::None();
	}
}

const itemgen::Config getItemGeneratorConfig(const DropGenerationInput & input, const EItemRarity & rarity) {
	return itemgen::Config(
		input.Predicate,
		getPowerRangeCalc(rarity, input.DropItemSource),
		getEnchantmentCalc(input.DropItemSource, input.SourceType),
		getProbabilityCalc(rarity),
		itemgen::powercalcs::Default(),
		getNetheriteEnchantmentCalc(input.DropItemSource, input.SourceType)
	);
}

TOptional<FInventoryItemData> generateDroppedItem(const Settings& settings, const DropGenerationInput& input, const EItemRarity& rarity) {	
	return itemgen::generate(settings, getExistingTargetPlayerLooterState(input.TargetPlayer), getItemGeneratorConfig(input, rarity));
}


FInventoryItemData addDisplayRarity(FInventoryItemData generatedItem, const EItemRarity& rarity) {
	auto fixedRarity = GetItemRegistry().Get(generatedItem.GetItemId()).getFixedRarity();

	// Some items doesn't allow random rarity (consumables, unique gear etc.) so use their fixed rarity instead.
	generatedItem.Rarity = fixedRarity.IsSet() ? fixedRarity.GetValue() : rarity;

	return generatedItem;
}

TOptional<itemgen::PowerRange> getPowerRangeMerged(UWorld* world, const DropGenerationInput& input) {
	const auto settings = getSettings(world);
	TOptional<itemgen::PowerRange> mergedRanged;
	for (auto rarity : input.itemRarityChance.GetPossibleRarities()) {
		auto rarityRange = getPowerRange(settings, getExistingTargetPlayerLooterState(input.TargetPlayer), getItemGeneratorConfig(input, rarity).powerrange);
		if (mergedRanged.IsSet()) {
			mergedRanged = mergeRanges(mergedRanged.GetValue(), rarityRange);
		} else {
			mergedRanged = rarityRange;
		}
	}
	return mergedRanged;
}

TOptional<FInventoryItemData> generateDroppedItem(UWorld* world, const DropGenerationInput& input) {
	const auto settings = getSettings(world);
	
	for (auto rarityIndex = calculateRarityIndex(input); rarityIndex >= 0; rarityIndex--) {
		auto rarity = asRarity(rarityIndex);
		auto oldRarity = rarity;

		// Each enchantment that needs to will try to generate a new item
		if(auto testData = generateDroppedItem(settings, input, rarity)) {
			if(testData->GetItemType().isGear()) {
				if (TOptional<UGearUtil*> GearUtil = DungeonsGearUtilLibrary::ChangeItemRarity(input.TargetPlayer, rarity)) {
					if (GearUtil.IsSet()) {
						FRareItemChance Chances = FRareItemChance({ { rarity, 100.0f } });
						if (auto itemData = generateDroppedItem(settings, DropGenerationInput(input.TargetPlayer, input.Predicate, Chances, input.DropItemSource), rarity)) {
							GearUtil.Get(nullptr)->OnLuckChanged(FPredictionKey());
							return addDisplayRarity(itemData.GetValue(), rarity);
						}
					}
					rarity = oldRarity;
				}
			}
			return addDisplayRarity(testData.GetValue(), rarity);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Unable to generate dropped item."));
	return {};
}

TArray<FInventoryItemData> generateDroppedItems(UWorld* world, const DropGenerationInput& input, const int itemCount) {
	TArray<FInventoryItemData> result;

	for (auto index = 0; index < itemCount; index++) {
		if(auto entry = generateDroppedItem(world, input)) {
			result.Add(entry.GetValue());
		}
	}		
	
	return result;
}


TArray<FInventoryItemData> generateDroppedEmeralds(UWorld* world, const DropGenerationInput& input, const int itemCount)
{
	TArray<FInventoryItemData> result;

	TOptional<FInventoryItemData> EmeraldData = generateDroppedItem(world, input);
	
	int CurrentValue = 1;
	EmeraldData.GetValue().OverrideStoreCount = CurrentValue;

	int iCurrentItems = itemCount;
	int iStep = sEmeraldValueMergeStepCount;

	while (iCurrentItems > CurrentValue)
	{	
		int iPos = (result.Num()) ? (FMath::Rand() % result.Num()) : 0;

		result.Insert(EmeraldData.GetValue(), iPos);
		iCurrentItems -= CurrentValue;

		if (!iStep--)
		{
			++CurrentValue;
			EmeraldData.GetValue().OverrideStoreCount = CurrentValue;
			iStep = sEmeraldValueMergeStepCount;
		}
	}

	//add leftover
	if (iCurrentItems > 0)
	{
		EmeraldData.GetValue().OverrideStoreCount = iCurrentItems;
		result.Add(EmeraldData.GetValue());
	}


	return result;
}

}}}
