#include "Dungeons.h"
#include "AnimActions.h"
#include "StateActions.h"
#include "game/actor/character/mob/MobCharacter.h"

namespace bt { namespace anim {

Action PlayBodySequence(UAnimSequenceBase* NewSequence, FName slot, float blendInTime, float blendOutTime, float PlayRate, int LoopCount, float BlendOutTriggerTime, float InTimeToStartMontageAt) {
	return [=](StateRef state) { 
		state.owner->MulticastPlayAnimationAsDynamicMontage(NewSequence, slot, blendInTime, blendOutTime, PlayRate, LoopCount, BlendOutTriggerTime, InTimeToStartMontageAt, FPredictionKey());
	};
}

Action SetLookAtTarget(actor::Provider targetProvider) {
	return [provider = std::move(targetProvider)](StateRef state) {
		state.owner->SetLookAtActor(provider(state));
	};
}

const Action& ClearLookAtTarget() {
	static const Action action = [](StateRef state) {
		state.owner->ResetLookAtActor();
	};
	return action;
}

}}
