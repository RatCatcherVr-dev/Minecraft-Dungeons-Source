#include "Dungeons.h"
#include "BeamAttack.h"
#include "game/actor/character/BaseCharacterStates.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/provider/Actors.h"
#include "game/component/MeleeAttackComponent.h"
#include "game/component/BeamAttackComponent.h"
#include "game/ai/bt/BtEvalState.h"

UBeamAttack::UBeamAttack(bool blind) : blind(blind) {
	type = "aoe-attack";
}

UBeamAttack::UBeamAttack(const bt::Provider<AActor*>&, int maxAttacksToIssue) {
	type = "aoe-attack";
}

void UBeamAttack::Init(bt::StateRef state) {
	attackComponent = state.owner->FindComponentByClass<UBeamAttackComponent>();
}

bool UBeamAttack::OnCanRun(bt::StateRef state) {
	if (blind && attackComponent) { return true; }
	return attackComponent != nullptr && attackComponent->CanAttack();
}

bool UBeamAttack::OnCanContinue(bt::StateRef state) {
	if (blind) {
		return !isDone || !(endTime.IsPassed(state));
	}
	else {
		return !isDone || !(endTime.IsPassed(state) && !attackComponent->IsAttacking());
	}
}

void UBeamAttack::OnStart(bt::StateRef state) {
	isDone = false;

	startTime = state.world().GetTimeSeconds();
	endTime = bt::TimeStamp::SecondsFromNow(state, attackComponent->delayTime + attackComponent->GetFullBeamTime());
}

void UBeamAttack::OnTick(bt::StateRef state) {
	if (blind) {
		if (isDone || attackComponent->IsAttackInProgress()) {
			return;
		}
		currentAttackCounter = attackComponent->SuccessfulAttackCounter;
		attackComponent->AttackLocal(nullptr);
		state.owner->SetAttackState(EAttackState::Melee);
		state.params().lastAttackTime = bt::TimeStamp::Now(state);
		isDone = true;
	}
	else {
		if (!attackComponent->CanAttack() || isDone) {
			return;
		}

		if (attackComponent->CanAttack(nullptr)) {
			currentAttackCounter = attackComponent->SuccessfulAttackCounter;
			attackComponent->AttackLocal(nullptr);
			state.owner->SetAttackState(EAttackState::Melee);
			state.params().lastAttackTime = bt::TimeStamp::Now(state);
			isDone = true;
		}
	}
}

void UBeamAttack::OnStop(bt::StateRef state) {
	state.owner->SetAttackState(EAttackState::None);
	attackComponent->Stop();
}