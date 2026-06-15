#include "Dungeons.h"
#include "SpeedActions.h"
#include "game/ai/bt/BtEvalState.h"
#include "game/actor/character/mob/MobCharacter.h"

namespace bt { namespace speed {

Action SetAbsolute(float speed) {
	return [=](StateRef state) {
		float movespeed = state.owner->CalculateAbsoluteMoveSpeed(speed);
		state.movement->MaxWalkSpeed = movespeed;
		state.movement->MaxFlySpeed = movespeed;
		state.owner->setIntendedAnimationSpeed(movespeed);
	};
}

Action SetAbsolute(const Provider<float>& speedProvider) {
	return [=](StateRef state) {
		float movespeed = state.owner->CalculateAbsoluteMoveSpeed(speedProvider(state));
		state.movement->MaxWalkSpeed = movespeed;
		state.movement->MaxFlySpeed = movespeed;
		state.owner->setIntendedAnimationSpeed(movespeed);
	};
}

Action SetRelative(float relativeFraction) {
	return [=](StateRef state) {
		float movespeed = state.owner->CalculateRelativeMoveSpeed(relativeFraction);
		state.movement->MaxWalkSpeed = movespeed;
		state.movement->MaxFlySpeed = movespeed;
		state.owner->setIntendedAnimationSpeed(movespeed);
	};
}

Action SetRelative(EMovementState movement, float relative/*= 1.0f*/) {
	float stateRelative = relative;
	if (movement == EMovementState::Walking) {
		stateRelative *= 0.4f;
	} else if (movement == EMovementState::Dashing) {
		stateRelative *= 1.2f;
	}
	return [=](StateRef state) { 
		auto movespeed = state.owner->CalculateRelativeMoveSpeed(stateRelative);
		state.movement->MaxWalkSpeed = movespeed;
		state.movement->MaxFlySpeed = movespeed;
		state.owner->setIntendedAnimationSpeed(movespeed);
	};
}

Action Set(const Speed& speed) {
	if (speed.absolute) {
		return SetAbsolute(speed.absolute.GetValue());
	}
	if (speed.movement) {
		return SetRelative(speed.movement.GetValue(), speed.relative.GetValue());
	}
	return SetRelative(speed.relative.GetValue());
}
}}
