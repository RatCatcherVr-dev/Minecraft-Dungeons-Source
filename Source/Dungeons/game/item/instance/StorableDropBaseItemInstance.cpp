#include "StorableDropBaseItemInstance.h"
#include "Dungeons.h"
#include "game/Game.h"
#include "game/actor/item/ItemEmitterActor.h"
#include "game/actor/item/StorableItem.h"

int AStorableDropBaseItemInstance::DropItems(int dropAmount, bool forceDuplicate, bool preventDuplicate) {
	if (dropAmount > 0) {
		using namespace game::item::util;
		// D11.DB - Ensure mission restrictions apply (some drops are restricted by DLC levels).
		TArray<FSpecifiedStoreCountItemDrop> possibleItems;
		if (const auto game = actorquery::getGame(GetWorld())) {
			for (auto& item : PossibleItemsToDrop) {
				const auto& type = GetItemRegistry().Get(item.Id);
				if (type.isAllowedOnMissionDifficulty(game->settings().levelName, game->settings().difficulty)) {
					possibleItems.Add(item);
				}
			}
		}

		//D11.SC pregenerate a drop list and pass it off to the emitter to load and process
		if (possibleItems.Num() > 0) {

			TArray<FSpecifiedStoreCountItemDrop> GeneratedItemsData;
			for (int i = 0; i < dropAmount; ++i) {
				int index = FMath::RandRange(0, possibleItems.Num() - 1);
				GeneratedItemsData.Push(possibleItems[index]);
				if (forceDuplicate) {
					GeneratedItemsData.Push(possibleItems[index]);
					i++;
				}
				else if (preventDuplicate) {
					possibleItems.RemoveAt(index);
				}
			}

			AItemEmitterActor::SpawnSimpleItemEmitter(GetWorld(), GeneratedItemsData, GetOwner()->GetActorLocation(), GetOwner(), false, false, true, FItemEmitterActorCompleteDelegate::CreateLambda([](TArray<AStorableItem*>& SpawnedItems) {
				for (auto item : SpawnedItems) {
					item->SetReplicates(true);
					item->SetActorTickEnabled(true);
				}
				SpreadOutItemsInSquare(SpawnedItems);
			}));

			return GeneratedItemsData.Num();
		}
	}
	return 0;
}