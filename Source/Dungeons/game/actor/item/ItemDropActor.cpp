#include "Dungeons.h"
#include "ItemDropActor.h"
#include "game/item/drop/ItemDropGenerator.h"
#include "game/actor/item/ItemEmitterActor.h"
#include "AbilitySystemComponent.h"
AItemDropActor::AItemDropActor() {
	PrimaryActorTick.bCanEverTick = false;
}

void AItemDropActor::DropItems(const FVector& spawnLocation, const FSerializableItemId& itemType, const int32 dropCount, AActor* targetPlayer, bool lockItemToOwner) const {
	const auto predicate = itemgen::predicates::Only(itemType);
	const auto rareItemChance = FRareItemChance::GetAdjustedChanceFromCategory(EItemRarityChanceCategory::Default, targetPlayer);


	const game::item::drop::DropGenerationInput rewardData(targetPlayer, predicate, rareItemChance, itemgen::ItemSource::Drop);


	auto generatedItems = generateDroppedItems(GetWorld(), rewardData, dropCount);
	AItemEmitterActor::SpawnSimpleItemEmitter(GetWorld(), generatedItems, spawnLocation, targetPlayer, lockItemToOwner, true, false, FItemEmitterActorCompleteDelegate::CreateLambda([](TArray< AStorableItem* > & SpawnedItems) {
		for (auto item : SpawnedItems) {
			item->ApplyDropEffect(nullptr);
		}
	}));
}
