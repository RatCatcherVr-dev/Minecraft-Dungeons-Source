#include "Dungeons.h"
#include "ItemDropData.h"
#include "game/item/generator/ItemGeneratorConfigs.h"
#include "game/util/ActorQuery.h"
#include "game/difficulty/DifficultyStats.h"
#include "game/Game.h"
#include "game/GameSettings.h"
#include "ItemDropGenerator.h"
#include "game/actor/character/mob/MobCharacter.h"

namespace itemdrop {
	TOptional<EDropGeneratorCategory> generatorCategoryFromCategory(EDropCategory category) {
		switch (category)
		{
		case EDropCategory::Food:
			return EDropGeneratorCategory::Food;
		case EDropCategory::Consumable:
			return EDropGeneratorCategory::Consumable;
		case EDropCategory::ConsumableNoWaterBreathing:
			return EDropGeneratorCategory::ConsumableNoWaterBreathing;
		case EDropCategory::Arrow:
			return EDropGeneratorCategory::Arrow;
		case EDropCategory::Emerald:
			return EDropGeneratorCategory::Emerald;
		case EDropCategory::Token:
			return EDropGeneratorCategory::Token;
		case EDropCategory::Gold:
			return EDropGeneratorCategory::Gold;
		case EDropCategory::Gear:
			return EDropGeneratorCategory::Gear;
		case EDropCategory::PermanentItem:
			return EDropGeneratorCategory::PermanentItem;
		case EDropCategory::None:
		default:
			break;
		}

		return {};
	}

	TOptional<EDropSpawnType> spawnTypeFromCategory(EDropCategory category) {
		switch (category)
		{
		case EDropCategory::Food:
		case EDropCategory::Consumable:
		case EDropCategory::ConsumableNoWaterBreathing:
		case EDropCategory::Arrow:
		case EDropCategory::Token:
			return EDropSpawnType::Shared;
		case EDropCategory::Emerald:
			return EDropSpawnType::All;
		case EDropCategory::Gold:
		case EDropCategory::Gear:
		case EDropCategory::PermanentItem:
			return EDropSpawnType::Individual;
		default:
			break;
		}

		return {};
	}
}

FDropGeneratorData::FDropGeneratorData(EDropGeneratorCategory categrory, TArray<FSerializableItemId> table)
	: GeneratorCategory(categrory), DropTable(MoveTemp(table))
{
}

const game::item::generator::Pred FDropGeneratorData::pred() const
{
	switch (GeneratorCategory)
	{
	case EDropGeneratorCategory::Food:
		return game::item::drop::Food();
	case EDropGeneratorCategory::Consumable:
		return game::item::drop::Consumable();
	case EDropGeneratorCategory::ConsumableNoWaterBreathing:
		return game::item::drop::ConsumableNoWaterBreathing();
	case EDropGeneratorCategory::Arrow:
		return game::item::drop::Arrow();
	case EDropGeneratorCategory::Emerald:
		return game::item::drop::Emerald();
	case EDropGeneratorCategory::Gold:
		return game::item::drop::Gold();
	case EDropGeneratorCategory::Gear:
		return itemgen::predicates::Gear();
	case EDropGeneratorCategory::PermanentItem:
		return itemgen::predicates::Item();
	case EDropGeneratorCategory::Token:
	case EDropGeneratorCategory::LootTable:
	{
		TArray<FItemId> itemIds;
		for (const auto& id : DropTable) {
			itemIds.Add(id);
		}
		return itemgen::predicates::OneOf(itemIds);
	}
	case EDropGeneratorCategory::LootTableIncludeChildren:
		TArray<FItemId> itemIds;
		TSet<FItemId> baseItems;
		for (const auto& id : DropTable) {
			itemIds.Add(id);
			baseItems.Add(id);
		}

		//Filter out items that have the base items as parents as well.
		for (const auto& type : GetItemRegistry().GetValues()) {
			if (!type->isEventItem()) {
				if (auto parent = type->getParent()) {
					if (baseItems.Contains(parent->getId())) {
						itemIds.Add(type->getId());
					}
				}
			}
		}

		return itemgen::predicates::OneOf(itemIds);
	}

	return itemgen::predicates::None();
}

TOptional<FNetworkedItemDropData> FDropCategoryDescription::GetDropData() const {
	const auto maxDropCount = Category == EDropCategory::Emerald ? 5 : 1;
	return FNetworkedItemDropData::FromFItemDrop(FItemDrop(Category, 1, maxDropCount));
}

FNetworkedItemDropData::FNetworkedItemDropData(EDropGeneratorCategory category, TArray<FSerializableItemId> table, EDropSpawnType type, int32 min, int32 max, float probability)
	: DropData(category, table)
	, SpawnType(type)
	, MinAmount(min)
	, MaxAmount(max)
	, Probability(probability)
{
}

TOptional<FNetworkedItemDropData> FNetworkedItemDropData::FromFItemDrop(const FItemDrop& itemDrop) {
	auto category = itemdrop::generatorCategoryFromCategory(itemDrop.Category);
	auto spawnType = itemdrop::spawnTypeFromCategory(itemDrop.Category);

	if (category && spawnType) {
		return FNetworkedItemDropData(category.GetValue(), {}, spawnType.GetValue(), itemDrop.MinAmount, itemDrop.MaxAmount, itemDrop.Probability);
	}

	return {};
}

bool FNetworkedItemDropData::ShouldDropRandomized(float probabilityMultiplier) const {
	const float modifiedProbability = Probability * probabilityMultiplier;
	return modifiedProbability == 1.0f || FMath::FRandRange(0.0f, 1.0f) < FMath::Clamp(modifiedProbability, 0.0f, 1.0f);
}

int32 FNetworkedItemDropData::GetDropCount(UWorld* world) const {
	int rolledAmount = MinAmount != MaxAmount ? FMath::RandRange(MinAmount, MaxAmount) : MaxAmount;
	if (DropData.GeneratorCategory == EDropGeneratorCategory::Emerald) {
		if (auto game = actorquery::getGame(world)) {
			rolledAmount = static_cast<float>(rolledAmount) * game->settings().difficultyStats.GetEmeraldDropMultiplier();
		}
	}
	return rolledAmount;
}

float FNetworkedItemDropData::GetProbability() const {
	return Probability;
}

FItemDropSource::FItemDropSource(const FVector dropLocation, AActor * sourceActor, APlayerCharacter * triggeringPlayer)
	: DropLocation(dropLocation), TriggeringPlayer(triggeringPlayer) {
	if(auto* mob = Cast<AMobCharacter>(sourceActor)) {
		MobType = mob->EntityType;
	}
}
