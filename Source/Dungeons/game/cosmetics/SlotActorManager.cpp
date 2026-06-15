#include "Dungeons.h"
#include "SlotActorManager.h"
#include "game/component/DungeonsChildActorComponent.h"

void USlotActorManager::Register(AActor* owner, UObject* slot) {
	if (!SlotActors.Contains(slot)) {
		SlotActors.Add(slot, NewObject<UDungeonsChildActorComponent>(owner));
		SlotActors[slot]->RegisterComponent();
	}
}

void USlotActorManager::DestroyActor(UObject* slot) {
	if (SlotActors.Contains(slot)) {
		auto component = SlotActors[slot];
		component->DestroyChildActor();
		//4.22-update no longer needed for TStructOnScope - type?
		//delete component->GetComponentInstanceData();
	}
}

void USlotActorManager::RemoveSlot(UObject* slot) {
	if (SlotActors.Contains(slot)) {
		auto component = SlotActors[slot];
		component->DestroyChildActor();
		SlotActors.Remove(slot);
	}
}

UChildActorComponent* USlotActorManager::AssignActorClass(UObject* slot, const TSubclassOf<AActor> actorClass) {
	auto component = SlotActors[slot];
	if (actorClass) {
		component->SetChildActorClass(actorClass);	
	}	
	return component;
}

TMap<UObject*, UChildActorComponent*> USlotActorManager::GetActors() const {
	return SlotActors;
}

void USlotActorManager::EmptyActors() {
	SlotActors.Empty();
}
