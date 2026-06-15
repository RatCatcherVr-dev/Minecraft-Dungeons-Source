#include "Dungeons.h"
#include "MeleeAttackBehavior.h"
#include "NodeFactory.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/action/TransformActions.h"
#include "game/ai/condition/IsInRange.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "MoveBehavior.h"
#include "game/component/BehaviorOptionsComponent.h"

namespace bt { namespace behavior {

Unique<UBtNode> chaseAndMeleeAttack(AMobCharacter& mob, const UBehaviorOptionsComponent& options, const actor::Provider& target /* = bt::actor::Target()*/) {
	return selector(
		sequence("Attack",
			isInRange(
				actor::Target(),
				options.Get("AttackInDistance", 250.f),
				options.Get("AttackOutDistance", 400.f)
			),
			ifElse(isInRange(actor::Target(), options.Get("AttackDistance", 140.f)),
				behavior::meleeAttack(mob),
				behavior::moveTo(actor::Target(), Relative(1.1f))
			)
		),
		behavior::chaseInOffensiveRange(mob, actor::Target(), Relative(.9f))
	);
}

}}
