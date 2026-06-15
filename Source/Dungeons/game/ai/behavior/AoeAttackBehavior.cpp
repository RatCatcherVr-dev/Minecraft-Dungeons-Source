#include "Dungeons.h"
#include "NodeFactory.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/provider/Locators.h"
#include "game/ai/task/attack/AoeAttack.h"
#include "game/component/AoeAttackComponent.h"
#include "../action/FocusActions.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/ai/condition/AttackPredicates.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/BtPredicate.h"
#include "../bt/group/BtGroup.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/condition/IsInRange.h"

namespace bt { namespace behavior {

Unique<UBtGroup> aoeAttack(AMobCharacter& mob, FName ComponentTag) {
	return sequence("aoe-attack",
		std::make_unique<UAoeAttack>()
	);
}

Unique<UBtGroup> aoeAttackInOrientation(const ABaseCharacter& character, const actor::Provider& target /*= actor::Target()*/, int maxAttacksToIssue /*= 1*/)
{
	auto attackComponent = character.FindComponentByClass<UAoeAttackComponent>();
	if (!attackComponent) {
		return {};
	}
	return sequence("aoe-attack",
		focus::Set(target, EAIFocusPriority::Attacking),
		onStop({
			focus::Clear(EAIFocusPriority::Attacking)
			}),
		ifElse((actor::IsAlive(target) && attack::InAttackOrientation(target, attackComponent)) || attack::IsAttackInProgress(attackComponent),
			make_unique<UAoeAttack>(target, maxAttacksToIssue),
			actor::IsAlive(target)
		)
	);
}

}}
