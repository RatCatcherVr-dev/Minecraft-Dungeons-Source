#include "Dungeons.h"
#include "PlayAnimation.h"
#include "game/actor/character/mob/MobCharacter.h"

UPlayAnimation::UPlayAnimation(UAnimSequenceBase* sequence, bool loopForever /*= false*/, bool blend  /*= true*/, bool fire)
	: sequence { sequence }
	, loopForever { loopForever }
	, fire { fire }
	, blend { blend } {
}

bool UPlayAnimation::OnCanRun(bt::StateRef state) {
	return state.anim() != nullptr && sequence != nullptr;
}

bool UPlayAnimation::OnCanContinue(bt::StateRef state) {
	return loopForever || !endTime.IsPassed(state);
}

void UPlayAnimation::OnStart(bt::StateRef state) {	
	if (auto montage = Cast<UAnimMontage>(sequence)) {
		state.owner->MulticastPlayMontage(montage);
	} else {
		state.owner->MulticastPlayAnimationAsDynamicMontage(sequence, FName(TEXT("FullBody")), blend ? .2f : 0.f, blend ? .2f : 0.f, 1.f, loopForever ? -1 : 1);
	}

	if (!loopForever) {
		endTime = bt::TimeStamp::SecondsFromNow(state, sequence->GetPlayLength());
	}
	if( fire ) {
		endTime = bt::TimeStamp::Now(state);
	}
}

void UPlayAnimation::OnStop(bt::StateRef state) {
	if (loopForever) {
		if (auto montage = Cast<UAnimMontage>(sequence)) {
			state.owner->MulticastStopMontage(.2f, montage);
		} else {
			state.owner->MulticastStopMontage(.2f);
		}
	}
}

Unique<UPlayAnimation> playAnimation(UAnimSequenceBase* sequence, bool loopForever /*= false*/, bool blend, bool fire) {
	return make_unique<UPlayAnimation>(sequence, loopForever, blend, fire);
}
