#include "OxygenSourceComponent.h"
#include "DungeonsGameInstance.h"

AOxygenSourceComponent::AOxygenSourceComponent(const FObjectInitializer& objectInitializer) : Super(objectInitializer) {
	bReplicates = true;
}

void AOxygenSourceComponent::BeginPlay() {
	Super::BeginPlay();
	InstanceTracker<AOxygenSourceComponent>::AddInstance(GetWorld(), this);
}

void AOxygenSourceComponent::EndPlay(EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	InstanceTracker<AOxygenSourceComponent>::RemoveInstance(GetWorld(), this);
}