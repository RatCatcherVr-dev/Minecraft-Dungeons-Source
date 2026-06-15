#include "Dungeons.h"
#include "HealTask.h"
#include "game/actor/character/mob/MobCharacter.h"

UHealTask::UHealTask(
	UAnimSequenceBase* sequence,
	bt::Duration animationDuration,
	bt::Duration healDelay,
	bt::Duration healCooldown,
	float amount
)
	: sequence { sequence }
	, animationDuration { animationDuration }
	, healDelay { healDelay }
	, healCooldown { healCooldown }
	, amount { amount } {
}

bool UHealTask::OnCanRun(bt::StateRef state) {
	return earliestStartTime.IsPassed(state);
}

bool UHealTask::OnCanContinue(bt::StateRef state) {
	if (!healTime.IsPassed(state)) {
		return true;
	}

	if (!healed && TryHeal(state)) {
		healed = true;
		earliestStartTime = bt::TimeStamp::FromNow(state, healCooldown);		
	}

	return !animationEndTime.IsPassed(state);
}

void UHealTask::OnStart(bt::StateRef state) {
	healed = false;
	healTime = bt::TimeStamp::FromNow(state, healDelay);
	animationEndTime = bt::TimeStamp::FromNow(state, animationDuration);

	state.owner->MulticastPlayAnimationAsDynamicMontage(sequence, FName(TEXT("FullBody")));
	state.owner->RemoveInvisibility();
}

bool UHealTask::TryHeal(bt::StateRef state) {
	// can this ever fail?
	const auto healthComponent = state.owner->FindComponentByClass<UHealthComponent>();
	healthComponent->ApplyHeal(amount);
	
	return true;
}