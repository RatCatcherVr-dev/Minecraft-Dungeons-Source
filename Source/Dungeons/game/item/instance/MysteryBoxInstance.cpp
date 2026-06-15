#include "Dungeons.h"
#include "MysteryBoxInstance.h"
#include "game/inventory/InventoryItem.h"
#include "game/item/drop/ItemDropGenerator.h"
#include "game/item/power/ItemPowerUtil.h"
#include "game/component/ItemStashComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/util/ActorQuery.h"
#include "game/Game.h"

AMysteryBoxInstance::AMysteryBoxInstance(){}

game::item::drop::DropGenerationInput AMysteryBoxInstance::GetDropGenerationInput(const ABaseCharacter& owner) const {	
	auto dropPredicate = game::item::generator::predicates::VendorBoxSlotType(GeneratedItemSlotTypes);
	return game::item::drop::DropGenerationInput(&owner, dropPredicate, FRareItemChance::GetChanceFromCategory(GeneratedItemRarityChanceCategory), game::item::generator::ItemSource::MysteryBox);
}

void AMysteryBoxInstance::Activate(const FPredictionKey& predictionKey) {
	const auto owner = GetCharacterOwner();
	check(owner && "must have owner");
	if (!owner) { return; }

	auto itemStash = owner->FindComponentByClass<UItemStashComponent>();
	check(itemStash && "owner must have an item stash");
	if (!itemStash) { return; }
	
	auto maybeItem = game::item::drop::generateDroppedItem(GetWorld(), GetDropGenerationInput(*owner));
	if(maybeItem.IsSet()){
		auto itemDataToGive = maybeItem.GetValue();		
		itemDataToGive.OnAdded.BindLambda([ItemDataSource = OptionalItemDataSource](UInventoryItem* item) -> void {
			if (ItemDataSource.IsSet()) {
				auto data = ItemDataSource.GetValue();
				data.OnWasAdded(item);
			}
		});
		itemStash->ClientPickupItem(itemDataToGive);
	}
}

void AMysteryBoxInstance::PopulateBulletPoints(TArray<FItemBulletPoint>& intoList, const ABaseCharacter& owner) const {
	Super::PopulateBulletPoints(intoList, owner);
	auto maybeItemPowerRange = game::item::drop::getPowerRangeMerged(owner.GetWorld(), GetDropGenerationInput(owner));
	if(maybeItemPowerRange.IsSet()){		
		intoList.Add(FItemBulletPoint::CreateItemPowerRangeBulletPoint(maybeItemPowerRange.GetValue()));
	}	
}
