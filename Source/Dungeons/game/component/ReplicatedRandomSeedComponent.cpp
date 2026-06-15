// © 2020 Mojang Synergies AB. TM Microsoft Corporation.


#include "ReplicatedRandomSeedComponent.h"
#include "UnrealNetwork.h"

UReplicatedRandomSeedComponent::UReplicatedRandomSeedComponent() {
	PrimaryComponentTick.bCanEverTick = false;
	bReplicates = true;
}

void UReplicatedRandomSeedComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UReplicatedRandomSeedComponent, Seed);
}

void UReplicatedRandomSeedComponent::OnRep_Seed() {
	internalRandomStream = FRandomStream(Seed);
	internalRandom.setSeed(Seed);
}

void UReplicatedRandomSeedComponent::SetSeed(int32 seed) {
	if (GetOwnerRole() == ROLE_Authority) {
		Seed = seed;
		OnRep_Seed();
	}
}