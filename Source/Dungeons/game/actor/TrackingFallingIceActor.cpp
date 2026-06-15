#include "TrackingFallingIceActor.h"

ATrackingFallingIceActor::ATrackingFallingIceActor() {
	mTrackingComponent = CreateDefaultSubobject<UMoveToTargetMovementComponent>(TEXT("MoveToTargetComponent"));
	mTrackingComponent->SetComponentTickEnabled(false);
}

void ATrackingFallingIceActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATrackingFallingIceActor, mTrackingTarget)
}

void ATrackingFallingIceActor::SetTarget(AActor* target) {
	mTrackingTarget = target;
}

void ATrackingFallingIceActor::OnSummonPlayed() {
	// Now that our FX have finished, turn our collider on
	mBoxCollider->SetGenerateOverlapEvents(true);

	// Start Tracking
	if (mTrackingTarget) {
		Cast<UMoveToTargetMovementComponent>(mTrackingComponent)->SetTarget(mTrackingTarget);
		mTrackingComponent->SetComponentTickEnabled(true);

		GetWorld()->GetTimerManager().SetTimer(mTrackingTimerHandle, this, &ATrackingFallingIceActor::OnTrackingComplete, mTrackingTime);
	}
	else {
		// We don't have a target, just start our drop timer fall.
		OnTrackingComplete();
	}
}

void ATrackingFallingIceActor::OnTrackingComplete() {
	mTrackingComponent->SetComponentTickEnabled(false);
	GetWorld()->GetTimerManager().SetTimer(mDropTimerHandle, this, &AFallingIceActor::DropIceblock, mDropDelay);
}
