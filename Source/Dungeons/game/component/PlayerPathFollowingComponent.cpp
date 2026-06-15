#include "Dungeons.h"
#include "PlayerPathFollowingComponent.h"
#include "game/actor/character/BaseCharacter.h"

void UPlayerPathFollowingComponent::FollowPathSegment(float DeltaTime)
{
	if (!Path.IsValid() || MovementComp == nullptr)
	{
		return;
	}

	const FVector CurrentLocation = MovementComp->GetActorFeetLocation();
	const FVector CurrentTarget = GetCurrentTargetLocation();

	const bool bAccelerationBased = MovementComp->UseAccelerationForPathFollowing();
	if (bAccelerationBased)
	{
		CurrentMoveInput = (CurrentTarget - CurrentLocation).GetSafeNormal();

		if (MoveSegmentStartIndex >= DecelerationSegmentIndex)
		{
			const FVector PathEnd = Path->GetEndLocation();
			const float DistToEndSq = FVector::DistSquared(CurrentLocation, PathEnd);
			const float BrakingDistance = MovementComp->GetPathFollowingBrakingDistance(CachedBrakingMaxSpeed);
			const bool bShouldDecelerate = DistToEndSq < FMath::Square(BrakingDistance);
			if (bShouldDecelerate)
			{
				const float SpeedPct = FMath::Clamp(FMath::Sqrt(DistToEndSq) / BrakingDistance, 0.0f, 1.0f);
				CurrentMoveInput *= SpeedPct;
			}
		}

		PostProcessMove.ExecuteIfBound(this, CurrentMoveInput);
		MovementComp->RequestPathMove(CurrentMoveInput);
	}
	else
	{
		FVector MoveVelocity = (CurrentTarget - CurrentLocation) / DeltaTime;
		PostProcessMove.ExecuteIfBound(this, MoveVelocity);
		// note: this basically sets the movement input. This is a hack to be able to use client path finding/following with the CharacterMovementComponent for 
		MovementComp->RequestPathMove(MoveVelocity.GetSafeNormal());
	}
}

bool UPlayerPathFollowingComponent::HasReachedCurrentTarget(const FVector& CurrentLocation) const {
	if (MovementComp == NULL) {
		return false;
	}

	const FVector CurrentTarget = GetCurrentTargetLocation();
	const FVector CurrentDirection = GetCurrentDirection();

	// check if moved too far
	const FVector ToTarget = CurrentTarget - MovementComp->GetActorFeetLocation();	
	if (FVector::DotProduct(ToTarget, CurrentDirection) < 0.0f) {
		return true;
	}
	
	const float GoalRadius = 0.0f;

	//Increased Goal size to 2 blocks (about the unblocked size required for a player character), to allow reaching goals during falls and jumps.
	const float GoalHalfHeight = 100.0f;

	//Increased acceptable agent radius fraction to 50% since we are only using cylinders 
	//which should be able to traverse corners 'barely' at full radius, and 'comfortably' at 50% (25% of diameter)
	const float AgentRadiusFraction = 0.5f;

	return HasReachedInternal(CurrentTarget, GoalRadius, GoalHalfHeight, CurrentLocation, CurrentAcceptanceRadius, AgentRadiusFraction);
}

void UPlayerPathFollowingComponent::ResumeMove(FAIRequestID RequestID /* = FAIRequestID::CurrentRequest */) {
	Super::ResumeMove(RequestID);
	if (Status == EPathFollowingStatus::Moving) {
		RotatePawnTowardsTarget();
	}
}

void UPlayerPathFollowingComponent::SetMoveSegment(int32 SegmentStartIndex) {
	Super::SetMoveSegment(SegmentStartIndex);
	RotatePawnTowardsTarget();
}

void UPlayerPathFollowingComponent::UpdatePathSegment() {
	Super::UpdatePathSegment();
	RotatePawnTowardsTarget();
}

void UPlayerPathFollowingComponent::RotatePawnTowardsTarget() {
	FVector Direction = GetCurrentDirection();

	// D11.BC we need to correct rotation here as direction can be calculated without considering goal offset
	const auto baseChar = Cast<ABaseCharacter>(DestinationActor.Get());
	if (baseChar && Path.IsValid() && baseChar->HasCustomTargetableCapsules()) {
		Direction = (*CurrentDestination - *Path->GetPathPointLocation(MoveSegmentStartIndex)).GetSafeNormal();
	}

	if (AController* controller = Cast<AController>(GetOwner())) {
		if (Direction != PreviousFocus) {
			PreviousFocus = Direction;
			controller->SetControlRotation(PreviousFocus.ToOrientationRotator());
		}
	}
}