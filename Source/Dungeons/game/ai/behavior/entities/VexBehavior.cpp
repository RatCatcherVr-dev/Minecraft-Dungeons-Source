#include "Dungeons.h"
#include "VexBehavior.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/MeleeAttackBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/condition/AttackPredicates.h"
#include "game/component/MeleeAttackComponent.h"
#include "game/actor/character/mob/MobCharacter.h"

namespace bt { namespace behavior { namespace entities {

BehaviorTuple createVex(AMobCharacter& mob) { return BehaviorTuple(
	selector("Vex",
		sequence("Attack",
			isInRange(actor::Target(), 250, 400),
			ifElse(attack::InAttackRangeOrAttacking(actor::Target(), mob.FindComponentByClass<UMeleeAttackComponent>()),
				behavior::meleeAttack(mob),
				behavior::moveTo(move::withSettings(actor::Target(), move::defaultRequest().SetUsePathfinding(false)), Relative(0.5f))
			)
		),
		behavior::chaseInOffensiveRange(mob, move::withSettings(actor::Target(), move::defaultRequest().SetUsePathfinding(false)), Relative(0.5f)),
		behavior::defaultRoam()
	),
	every(0.5s,
		set(actor::Target(), actor::ClosestEnemy())
	));
}

}}}
