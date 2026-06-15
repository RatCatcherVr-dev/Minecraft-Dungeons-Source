#include "Dungeons.h"
#include "WearableCosmeticInfo.h"
#include "util/CollectionUtil.h"

void AWearableCosmeticInfo::Equip(UChildActorComponent* assignedChildActorComponent, USkeletalMeshComponent* playerMesh) {
	Super::Equip(assignedChildActorComponent, playerMesh);

	if (auto* wearableCosmetic = Cast<AWearableCosmetic>(assignedChildActorComponent->GetChildActor())) {
		wearableCosmetic->AttachToPlayerMesh(playerMesh);
		EquippedCosmetics.Add(wearableCosmetic);
	}	
}

void AWearableCosmeticInfo::Unequip() {
	util::collection::clearAndDestroy(EquippedCosmetics);
}

TSubclassOf<AActor> AWearableCosmeticInfo::GetChildActorClass() {
	return WearableCosmeticClass;
}
