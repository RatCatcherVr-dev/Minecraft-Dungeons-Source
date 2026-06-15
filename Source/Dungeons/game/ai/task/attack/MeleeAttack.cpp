#include "Dungeons.h"
#include "MeleeAttack.h"
#include "game/actor/character/BaseCharacterStates.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/ai/bt/BtEvalState.h"
#include "game/ai/provider/Actors.h"
#include "game/component/MeleeAttackComponent.h"
#include "game/actor/character/mob/MobCharacter.h"

UMeleeAttack::UMeleeAttack(const bt::actor::Provider& target, bool blind, int index)
	: targetProvider(target)
	, index(index)
	, blind(blind) {
	type = "melee-attack";
}

bool UMeleeAttack::OnCanRun(bt::StateRef state) {

	attackComponent = state.owner->FindComponentByClass<UMeleeAttackComponent>();
	if( blind && attackComponent ) { return true; }
	return attackComponent != nullptr && (attackComponent->CanAttack() || isDone);
}

bool UMeleeAttack::OnCanContinue(bt::StateRef state) {
	return !(attackComponent->CanAttack() && isDone);
}

void UMeleeAttack::OnStart(bt::StateRef state) {
	isDone = false;
	if( index != -1 ) {
		attackComponent->SetAttackVariantIndex(index);
	}
}

void UMeleeAttack::OnTick(bt::StateRef state) {
	if( blind ) {
		if( isDone || attackComponent->IsAttackInProgress()) {
			return;
		}

		currentAttackCounter = attackComponent->AttackCounter;
		attackComponent->AttackLocal();
		state.owner->SetAttackState( EAttackState::Melee );
		state.params().lastAttackTime = bt::TimeStamp::Now( state );
		isDone = true;
	}
	else {
		if (!attackComponent->CanAttack() || isDone) {
			return;
		}

		const auto target = targetProvider(state);
		if (!target) {
			isDone = true;
			return;
		}

		if (attackComponent->CanAttack(target)) {
			currentAttackCounter = attackComponent->AttackCounter;		
			attackComponent->AttackLocal(target);
			state.owner->SetAttackState(EAttackState::Melee);
			state.params().lastAttackTime = bt::TimeStamp::Now(state);
			isDone = true;
		}
	}
}

void UMeleeAttack::OnStop(bt::StateRef state) {
	state.owner->SetAttackState(EAttackState::None);
	attackComponent->Stop();
}
