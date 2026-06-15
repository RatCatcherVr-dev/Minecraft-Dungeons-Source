#include "Dungeons.h"
#include "ReturnTask.h"
#include "game/actor/character/mob/MobCharacter.h"

UReturnTask::UReturnTask(UTurnComponent* component, bt::Provider<float> angleProvider)
	: component(component)
	, angleProvider(std::move(angleProvider)) {
}

bool UReturnTask::OnCanRun(bt::StateRef state) {
	if (component->IsTurning()) {
		return false;
	}

	const auto currentAngle = state.owner->GetActorRotation().Yaw;
	targetAngle = component->WrapAngle(angleProvider(state));
	
	return !FMath::IsNearlyEqual(currentAngle, targetAngle);
}

bool UReturnTask::OnCanContinue(bt::StateRef state) {
	return component->IsTurning();
}

void UReturnTask::OnStart(bt::StateRef state) {
	component->Turn(targetAngle);
}