#include "Dungeons.h"
#include "TurnTask.h"
#include "game/actor/character/mob/MobCharacter.h"

UTurnTask::UTurnTask(
	const bt::locator::Provider& target,
	float rate,
	float toleranceDegrees
)
	: target { target }
	, rate { rate }
	, tolerance { FMath::Cos(FMath::DegreesToRadians(toleranceDegrees)) } {
}

bool UTurnTask::OnCanRun(bt::StateRef state) {
	if (const auto maybeTarget = target(state).GetLocationLike()) {
		const auto forward = state.owner->GetActorForwardVector();
		const auto delta = maybeTarget.GetValue() - state.owner->GetActorLocation();
		
		return FVector::DotProduct(forward, delta.GetSafeNormal()) < tolerance;
	}
	
	return false;
}

bool UTurnTask::OnCanContinue(bt::StateRef state) {
	return OnCanRun(state);
}

void UTurnTask::OnTick(bt::StateRef state) {
	if (const auto maybeTarget = target(state).GetLocationLike()) {
		const auto forward = state.owner->GetActorForwardVector();
		const auto delta = maybeTarget.GetValue() - state.owner->GetActorLocation();

		const auto direction = FMath::Sign(FVector::CrossProduct(forward, delta).Z);
		
		const auto angleIncrement { rate * state.world().GetDeltaSeconds() * direction };
		state.owner->AddActorLocalRotation(FRotator { 0.f, angleIncrement, 0.f }, false, nullptr, ETeleportType::None);
	}
}