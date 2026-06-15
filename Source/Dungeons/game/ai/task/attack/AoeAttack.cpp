#include "Dungeons.h"
#include "AoeAttack.h"
#include "game/actor/character/BaseCharacterStates.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/provider/Actors.h"
#include "game/component/MeleeAttackComponent.h"
#include "game/component/AoeAttackComponent.h"
#include "game/ai/bt/BtEvalState.h"

UAoeAttack::UAoeAttack(bool blind)
	:blind(blind)
{
	type = "aoe-attack";
}

UAoeAttack::UAoeAttack(FName ComponentTag, bool blind)
	: ComponentTag(ComponentTag)
	, blind( blind )
{
	type = "aoe-attack";
}

UAoeAttack::UAoeAttack(const bt::Provider<AActor*>& provider, FName ComponentTag /*= "" */, bool blind /*= false*/)
	: AttackProvider(provider)
	, ComponentTag(ComponentTag)
	, blind(blind)
{
	type = "aoe-attack";
}

UAoeAttack::UAoeAttack(const bt::Provider<AActor*>&, int maxAttacksToIssue /*= 1*/)
{
	type = "aoe-attack";
}

void UAoeAttack::Init(bt::StateRef state) {
	if (ComponentTag.IsNone())
	{
		attackComponent = state.owner->FindComponentByClass<UAoeAttackComponent>();
	}
	else
	{
		TArray<UActorComponent*> componentsByTag = state.owner->GetComponentsByTag(UAoeAttackComponent::StaticClass(), ComponentTag);
		if (componentsByTag.Num() > 0)
		{
			attackComponent = Cast<UAoeAttackComponent>(componentsByTag[0]);
		}
		else
		{
			attackComponent = state.owner->FindComponentByClass<UAoeAttackComponent>();
		}
	}
}

bool UAoeAttack::OnCanRun(bt::StateRef state) {	
	if( blind && attackComponent ) { return true; }
	return attackComponent != nullptr && attackComponent->CanAttack();
}

bool UAoeAttack::OnCanContinue(bt::StateRef state) {
	if( blind ) { 
		return !isDone || !( endTime.IsPassed( state ) );
	}
	else {
		return !isDone || !( endTime.IsPassed( state ) && !attackComponent->IsAttacking() );
	}
}

void UAoeAttack::OnStart(bt::StateRef state) {	
	isDone = false;

	startTime = state.world().GetTimeSeconds();
	endTime = bt::TimeStamp::SecondsFromNow(state, attackComponent->totalTime);
}

void UAoeAttack::OnTick(bt::StateRef state) {
	if( blind ) {
		if( isDone || attackComponent->IsAttackInProgress() ) {
			return;
		}
		currentAttackCounter = attackComponent->SuccessfulAttackCounter;
		attackComponent->AttackLocal(nullptr);
		state.owner->SetAttackState(EAttackState::Melee);
		state.params().lastAttackTime = bt::TimeStamp::Now(state);
		isDone = true;
	}
	else {
		if ((attackComponent &&!attackComponent->CanAttack())|| isDone) {
			return;
		}

		if (attackComponent->CanAttack(nullptr)) {
			currentAttackCounter = attackComponent->SuccessfulAttackCounter;
			attackComponent->AttackLocal(AttackProvider ? AttackProvider(state) : nullptr);
			state.owner->SetAttackState(EAttackState::Melee);
			state.params().lastAttackTime = bt::TimeStamp::Now(state);
			isDone = true;
		}
	}
}

void UAoeAttack::OnStop(bt::StateRef state) {	
	state.owner->SetAttackState(EAttackState::None);
	attackComponent->Stop();
}
