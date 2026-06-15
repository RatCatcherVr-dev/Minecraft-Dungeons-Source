#include "Dungeons.h"
#include "RangedAttack.h"
#include "AiController.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/bt/BtEvalState.h"
#include "game/component/AttackComponent.h"
#include "game/component/RangedAttackComponent.h"
#include "game/util/ActorQuery.h"
#include "game/component/BehaviorOptionsComponent.h"

URangedAttack::URangedAttack()
	: URangedAttack("")
{
}

URangedAttack::URangedAttack(FName ComponentTag)
	: freeAim{ true }
	, maxAttacks(1)
	, componentTag(ComponentTag)
{
	type = "ranged-attack";
}

URangedAttack::URangedAttack(const bt::Provider<AActor*>& target, int maxAttacksToIssue, FName ComponentTag)
	: URangedAttack(target, {}, maxAttacksToIssue, ComponentTag)
{
}

URangedAttack::URangedAttack(const bt::Provider<AActor*>& target, const bt::Provider<FVector>& origin, int maxAttacksToIssue, FName ComponentTag)
	: targetProvider(target)
	, originProvider(origin)
	, maxAttacks(maxAttacksToIssue)
	, componentTag(ComponentTag)
{
	type = "ranged-attack";
}

bool URangedAttack::OnCanRun(bt::StateRef state) {
	if (!(attackComponent && attackComponent->CanAttack())) {
		return false;
	}

	if (freeAim) {
		return true;
	}
	
	const auto target = targetProvider(state);
	return target != nullptr &&
		actorquery::getActorDistanceSquared(state.owner, target) <= attackComponent->GetAttackRangeSquared();
}

bool URangedAttack::OnCanContinue(bt::StateRef state) {
	auto stop = (attacksIssued >= maxAttacks) && postAttackTime.IsPassed(state) && !attackComponent->IsAttackInProgress() && currentAttackCounter.update(attackComponent->AttackCounter);
	return !stop;
}

void URangedAttack::Init(bt::StateRef state) {
	if (componentTag.IsNone()) {
		attackComponent = state.owner->FindComponentByClass<URangedAttackComponent>();
	}
	else {
		TArray<UActorComponent*> componentsByTag = state.owner->GetComponentsByTag(URangedAttackComponent::StaticClass(), componentTag);
		if (componentsByTag.Num() > 0) {
			attackComponent = Cast<URangedAttackComponent>(componentsByTag[0]);
		}
		else {
			attackComponent = state.owner->FindComponentByClass<URangedAttackComponent>();
		}
	}
	optionsComponent = state.owner->FindComponentByClass<UBehaviorOptionsComponent>();
}

void URangedAttack::OnStart(bt::StateRef state) {
	attacksIssued = 0;
	startTime = state.world().GetTimeSeconds();
}

void URangedAttack::OnStop(bt::StateRef state) {
	attackComponent->Stop();
}

void URangedAttack::OnTick(bt::StateRef state) {
	if (attacksIssued < maxAttacks) {
		_Attack(state);
	}
}

void URangedAttack::_Attack(bt::StateRef state) {
	if (attackComponent->CanAttack()) {
		currentAttackCounter = attackComponent->AttackCounter;
		
		const auto target = freeAim ? nullptr : targetProvider(state);
		
		if (originProvider) {
			attackComponent->OverrideOrigin(originProvider(state));
		}
		attackComponent->AttackLocal(target);
		
		attacksIssued++;
		state.params().lastAttackTime = bt::TimeStamp::Now(state);
		postAttackTime = bt::TimeStamp::FromNow(state, bt::Duration { bt::Seconds { optionsComponent->Get("RangedPostAttackDelay", 0.f) + attackComponent->GetAttackTimeTotal() } });
	}
}