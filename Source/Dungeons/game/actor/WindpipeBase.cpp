#include "WindpipeBase.h"
// © 2020 Mojang Synergies AB. TM Microsoft Corporation.


#include "WindPipeBase.h"
#include <UnrealNetwork.h>
#include "game/component/ReplicatedInteractableComponent.h"

AWindPipeBase::AWindPipeBase() {
	bReplicates = true;
	Interactable = CreateDefaultSubobject<UReplicatedInteractableComponent>(TEXT("Interactable"));
	Interactable->bOneTimeInteraction = false;
	Interactable->bEditableWhenInherited = true;
	DefaultSceneRootInternal = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	DefaultSceneRootInternal->bEditableWhenInherited = true;
	RootComponent = DefaultSceneRootInternal;
	NetUpdateFrequency = 5.0f;
}

void AWindPipeBase::BeginPlay() {
	Super::BeginPlay();
	//Avoid events on start.
	InternalAngle = ReplicatedAngle;

	//Orientation changes only on server
	if (HasAuthority()) {
		Interactable->OnReplicatedInteract.AddDynamic(this, &AWindPipeBase::Rotate);
	}
}


void AWindPipeBase::Rotate() {
	if (!HasAuthority() || DisableRotation) return;

	//First check if we are on our bounds from previous rotation, if so we change rotation direction (this is to ensure we replicate the correct rotation direction).
	if (bUseMinMax && 
		((FMath::IsNearlyEqual(ReplicatedAngle, MinAngle) && !RotateClockwise) 
		|| (FMath::IsNearlyEqual(ReplicatedAngle, MaxAngle) && RotateClockwise))) {
		RotateClockwise = !RotateClockwise;
	}

	//Apply angle delta
	ReplicatedAngle = FMath::UnwindDegrees(ReplicatedAngle + sign() * AngleDelta);
	
	//Clamp to range
	if (bUseMinMax) {
		ReplicatedAngle = FMath::ClampAngle(ReplicatedAngle, MinAngle, MaxAngle);
	}

	//Transform to 0,360 range.
	if (ReplicatedAngle < 0) {
		ReplicatedAngle = ReplicatedAngle + 360.f;
	}

	ForceNetUpdate();
	OnRep_ReplicatedAngle();
}

void AWindPipeBase::OnRep_ReplicatedAngle() {
	if (ReplicatedAngle != InternalAngle) {
		OnAngleChanged(ReplicatedAngle, GetDeltaAngle(ReplicatedAngle));
		InternalAngle = ReplicatedAngle;
	}
}

void AWindPipeBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const {
	DOREPLIFETIME(AWindPipeBase, ReplicatedAngle);
}

void AWindPipeBase::SetRotatationDirecition(bool clockwise) {
	if (HasAuthority()) {
		RotateClockwise = clockwise;
	}
}

float AWindPipeBase::GetDeltaAngle(float NewAngle) const {
	const float shortest = FMath::FindDeltaAngleDegrees(InternalAngle, NewAngle);
	return AngleDelta < 180.f ? shortest : 360.f - shortest;

}

void AWindPipeBase::SetAngleSpan(float minAngle, float maxAngle) {
	bUseMinMax = true;
	if (minAngle > MaxAngle) Swap(minAngle, maxAngle);

	MaxAngle = maxAngle;
	MinAngle = minAngle;
}