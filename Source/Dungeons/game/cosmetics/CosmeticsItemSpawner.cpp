#include "Dungeons.h"
#include "CosmeticsItemSpawner.h"
#include "game/item/instance/AItemInstance.h"

namespace game { namespace cosmetics {

	FActorSpawnParameters getSpawnParameters(AActor* owner) {
		auto spawnParameters = FActorSpawnParameters();
		spawnParameters.Owner = owner;
		return spawnParameters;
	}

	ACosmeticItemInfo* spawnCosmeticItemInfo(UWorld* world, AActor* owner, const TSubclassOf<ACosmeticItemInfo> cosmeticItemClass) {
		return world->SpawnActor<ACosmeticItemInfo>(cosmeticItemClass, getSpawnParameters(owner));
	}
}}
