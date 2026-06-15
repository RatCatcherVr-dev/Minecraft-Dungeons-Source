#include "UnrealRespawnCandidateComponent.h"

URespawnCandidateComponent::URespawnCandidateComponent() {
	bReplicates = false;
}

void URespawnCandidateComponent::BeginPlay() {
	Super::BeginPlay();
	
	if (RemoveOnStart) {
		RemoveFromRoot();
		DestroyComponent();
		return;
	}

	InstanceTracker< URespawnCandidateComponent >::AddInstance(GetWorld(), this);
}

void URespawnCandidateComponent::EndPlay(EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	InstanceTracker< URespawnCandidateComponent >::RemoveInstance(this->GetWorld(), this);
}