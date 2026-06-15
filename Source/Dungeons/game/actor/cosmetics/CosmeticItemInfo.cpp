#include "Dungeons.h"
#include "CosmeticItemInfo.h"
#include "game/cosmetics/CosmeticType.h"
#include "UnrealNetwork.h"

ACosmeticItemInfo::ACosmeticItemInfo() {
 	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bReplicates = true;
	bAlwaysRelevant = true;
	bOnlyRelevantToOwner = false;
}

void ACosmeticItemInfo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);	
	DOREPLIFETIME(ACosmeticItemInfo, ItemName);
	DOREPLIFETIME(ACosmeticItemInfo, Type);
}

void ACosmeticItemInfo::Equip(UChildActorComponent* assignedChildActorComponent, USkeletalMeshComponent* playerMesh) {
}

void ACosmeticItemInfo::Unequip() {
}

const FName& ACosmeticItemInfo::GetItemName() const {
	return ItemName;
}

ECosmeticType ACosmeticItemInfo::GetType() const {
	return Type;
}

TSubclassOf<AActor> ACosmeticItemInfo::GetChildActorClass() {
	return nullptr;
}

