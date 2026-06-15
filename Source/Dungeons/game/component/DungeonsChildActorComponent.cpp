#include "Dungeons.h"
#include "DungeonsChildActorComponent.h"

void UDungeonsChildActorComponent::OnRegister() {
	SetChildActorClass(nullptr);
	Super::OnRegister();
}

void UDungeonsChildActorComponent::OnUnregister() {
	Super::OnUnregister();
}