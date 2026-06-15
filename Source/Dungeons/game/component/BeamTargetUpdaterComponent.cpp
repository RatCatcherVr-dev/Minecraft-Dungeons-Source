// © 2020 Mojang Synergies AB. TM Microsoft Corporation.


#include "BeamTargetUpdaterComponent.h"

namespace beamupdater {
	void UpdateComponent(const FBeamUpdaterInfo& info, const FVector& location) {
		for (const auto index : info.EmitterIndecies) {
			info.System->SetBeamTargetPoint(index, location, 0);
		}
	}
}

FBeamUpdaterInfo::FBeamUpdaterInfo(UParticleSystemComponent* system, TArray<int32>&& indecies)
	: System(system), EmitterIndecies(MoveTemp(indecies))
{}

UBeamTargetUpdaterComponent::UBeamTargetUpdaterComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBeamTargetUpdaterComponent::AddParticleSystemComponent(UParticleSystemComponent* Component, TArray<int32> Indecies) {

	if (ParticleSystems.AddUnique(FBeamUpdaterInfo(Component, MoveTemp(Indecies))) != INDEX_NONE && CurrentTarget) {
		beamupdater::UpdateComponent(ParticleSystems.Last(), CurrentTarget.GetValue());
	}
}

void UBeamTargetUpdaterComponent::RemoveParticleSystemComponent(UParticleSystemComponent* Component) {
	ParticleSystems.RemoveSwap(FBeamUpdaterInfo(Component, {}));
}


void UBeamTargetUpdaterComponent::SetBeamLocation(const FVector Location) {
	//if (CurrentTarget == Location) return;

	CurrentTarget = Location;

	for (const auto beamInfo : ParticleSystems) {
		beamupdater::UpdateComponent(beamInfo, Location);
	}
}

UActorBeamTargetUpdaterComponent::UActorBeamTargetUpdaterComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UActorBeamTargetUpdaterComponent::SetActorTarget(const AActor* targetActor) {
	TargetActor = targetActor;
	if(TargetActor.IsValid()) {
		SetBeamLocation(TargetActor->GetActorLocation());
		SetComponentTickEnabled(true);
	} else {
		SetComponentTickEnabled(false);
	}
}

void UActorBeamTargetUpdaterComponent::ForceUpdate() {
	if (const auto* target = TargetActor.Get()) {
		SetBeamLocation(target->GetActorLocation());
	}
}

void UActorBeamTargetUpdaterComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (const auto* target = TargetActor.Get()) {
		SetBeamLocation(target->GetActorLocation());
	}
	else {
		SetComponentTickEnabled(false);
	}
}
