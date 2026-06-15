#include "Dungeons.h"
#include "NodeFactory.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/ai/action/FocusActions.h"
#include "game/ai/action/CommonActions.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/condition/AttackPredicates.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/provider/Move.h"
#include "game/ai/task/attack/RangedAttack.h"
#include "game/component/RangedAttackComponent.h"
#include "util/FloatRange.h"

namespace bt { namespace behavior {

Unique<UBtGroup> rangedAttackInOrientation(const ABaseCharacter& character, FName componentTag, const actor::Provider& target /*= actor::Target()*/, int maxAttacksToIssue /*= 1*/) {
	URangedAttackComponent* attackComponent = nullptr;
	TArray<UActorComponent*> componentsByTag = character.GetComponentsByTag(URangedAttackComponent::StaticClass(), componentTag);
	if (componentsByTag.Num() > 0) {
		attackComponent = Cast<URangedAttackComponent>(componentsByTag[0]);
	}
	else {
		attackComponent = character.FindComponentByClass<URangedAttackComponent>();
	}
	return sequence("ranged-attack",
		focus::Set(target, EAIFocusPriority::Attacking),
		onStop({
			focus::Clear(EAIFocusPriority::Attacking)
			}),
		ifElse((actor::IsAlive(target) && attack::InAttackOrientation(target, attackComponent)) || attack::IsAttackInProgress(attackComponent),
			make_unique<URangedAttack>(target, maxAttacksToIssue, componentTag),
			actor::IsAlive(target)
		)
	);
}


Unique<UBtGroup> rangedAttackInOrientation(const ABaseCharacter& character, const actor::Provider& target/*= actor::Target()*/, int maxAttacksToIssue /* = 1*/) {
	return rangedAttackInOrientation(character, {}, target, maxAttacksToIssue);
}


Unique<UBtGroup> rangedAttackIfOriented(const ABaseCharacter& character, const actor::Provider& target/*= actor::Target()*/, int maxAttacksToIssue /* = 1*/) {
	auto attackComponent = character.FindComponentByClass<URangedAttackComponent>();
	if (!attackComponent) {
		return {};
	}
	return sequence("ranged-attack-if-oriented",
		startPredicate(attack::InAttackOrientation(
			target,
			character.FindComponentByClass<URangedAttackComponent>()
		)),
		make_unique<URangedAttack>(target, maxAttacksToIssue)
		);
}


Unique<UBtGroup> rangedAttack(const ABaseCharacter& character, float minAttackDistance, float acquireTargetRangeUnits, float loseTargetRangeUnits, const actor::Provider& target/*= actor::Target()*/, int maxAttacksToIssue /* = 1*/) {
	auto attackComponent = character.FindComponentByClass<URangedAttackComponent>();
	if (!attackComponent) {
		return {};
	}
	return sequence("ranged-attack",
		isInRange(target, FloatRange(minAttackDistance, acquireTargetRangeUnits), FloatRange(minAttackDistance, loseTargetRangeUnits)),	
		rangedAttackInOrientation(character, target, maxAttacksToIssue)
	);
}

Unique<UBtGroup> rangedAttackAndMoveCloser(const ABaseCharacter& character, float minAttackDistance, float acquireTargetRangeUnits, float loseTargetRangeUnits, float minMoveCloserDistance, const actor::Provider& target/*= actor::Target()*/, const speed::Speed& speed /*= Relative(0.5f)*/, int maxAttacksToIssue /* = 1*/) {
	auto attack = rangedAttack(character, minAttackDistance, acquireTargetRangeUnits, loseTargetRangeUnits, target, maxAttacksToIssue);
	if (!attack) {
		return{};
	}
	if (minMoveCloserDistance < 0) {
		minMoveCloserDistance = 1.2f * minAttackDistance;
	}
	return greedySequence("ranged-attack & move-closer",
		std::move(attack),
		sequence("move-closer",
			isInRange(target, 2 * acquireTargetRangeUnits),
			behavior::moveTo(move::withSettings(target, move::defaultRequest().SetAcceptanceRadius(minMoveCloserDistance)), speed)
		)
	);
}

}}
