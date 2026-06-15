#include "Dungeons.h"
#include "CosmeticsDisplayComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"

UCosmeticsDisplayComponent::UCosmeticsDisplayComponent() {
	bReplicates = true;
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;	
	SlotActorManager = CreateDefaultSubobject<USlotActorManager>(TEXT("SlotManager"));
}

void UCosmeticsDisplayComponent::BeginPlay() {
	Super::BeginPlay();
}

void UCosmeticsDisplayComponent::EndPlay(EEndPlayReason::Type endPlayReason) {
	UnbindSlots();
	SlotActorManager->EmptyActors();
}

void UCosmeticsDisplayComponent::SetOverridingPlayerMesh(USkeletalMeshComponent* playerMesh) {
	OverridingPlayerMesh = playerMesh;
}

TArray<UCosmeticsSlot*> UCosmeticsDisplayComponent::GetBoundSlots()
{
	return BoundSlots;
}

void UCosmeticsDisplayComponent::BindToSlots(const TArray<UCosmeticsSlot*> slots) {
	UnbindSlots();
	BindSlots(slots);
}

void UCosmeticsDisplayComponent::BindSlots(const TArray<UCosmeticsSlot*> slots) {
	for (auto* slot : slots) {
		if (slot) {
			SlotActorManager->Register(GetOwner(), slot);
			OnStoreItemUpdated(slot);
			slot->OnCosmeticsSlotUpdatedInternal.AddUObject(this, &UCosmeticsDisplayComponent::OnStoreItemUpdated);
			BoundSlots.Add(slot);	
		}		
	}
}

void UCosmeticsDisplayComponent::UnbindSlots() {
	for (auto* slot : BoundSlots) {
		if (slot) {
			SlotActorManager->DestroyActor(slot);
			SlotActorManager->RemoveSlot(slot);
			if (slot->OnCosmeticsSlotUpdatedInternal.IsBound()) {
				slot->OnCosmeticsSlotUpdatedInternal.RemoveAll(this);
			}	
		}		
	}	
	BoundSlots.Empty();
}

void UCosmeticsDisplayComponent::OnStoreItemUpdated(UCosmeticsSlot* slot) const {
	if (slot) {
		SlotActorManager->DestroyActor(slot);
		if (auto* cosmeticItem = slot->GetItem()) {
			cosmeticItem->Equip(SlotActorManager->AssignActorClass(slot, cosmeticItem->GetChildActorClass()), GetPlayerMesh());
		}	
	}	
}

USkeletalMeshComponent* UCosmeticsDisplayComponent::GetPlayerMesh() const {
	return OverridingPlayerMesh ? OverridingPlayerMesh : Cast<APlayerCharacter>(GetOwner())->GetMesh();;
}
