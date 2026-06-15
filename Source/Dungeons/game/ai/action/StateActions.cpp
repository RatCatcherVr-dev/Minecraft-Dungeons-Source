#include "Dungeons.h"
#include "StateActions.h"
#include "game/actor/character/mob/MobCharacter.h"

namespace bt { namespace state {

void setLocomotion(StateRef state, EAnimLocomotion locomotion) {
	if (auto anim = state.anim()) {
		anim->StateMachineConditions.FallBackLocomotionBlend = locomotion;
	}
}

Action SetLocomotion(EAnimLocomotion locomotion) {
	return [locomotion](StateRef state) {
		setLocomotion(state, locomotion);
	};
}


Action SetAttack(EAttackState attack) {
	return [attack](StateRef state) { state.owner->SetAttackState(attack); };
}

Action SetEventMobRevealed(bool show) {
	return [show](StateRef state) { state.owner->SetEventMobRevealed(show); };
}

// D11.DB
Action SetCollisionEnabled(bool enabled) {
	return [enabled](StateRef state) {
		state.owner->SetActorEnableCollision(enabled);
	};
}

Action SetIsTargetable(bool enabled) {
	return [enabled](StateRef state) {
		state.owner->SetTargetable(enabled);
	};
}

Action SetHealthBarEnabled( bool enabled ) {
	return [enabled](StateRef state) {
		if( auto hbc = state.owner->GetHealthBarComponent() ) {
			state.owner->EnableHealthBar = enabled;
			hbc->RefreshHidden();
		}
	};
}
// D11.DB - END

}}
