#include "Dungeons.h"
#include "BabyZombieBehavior.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/FleeBehavior.h"
#include "game/ai/behavior/MeleeAttackBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/provider/BtRandom.h"
#include "game/actor/character/mob/MobCharacter.h"

namespace bt { namespace behavior { namespace entities {

BehaviorTuple createBabyZombie(AMobCharacter& mob) { return BehaviorTuple(
	selector("Baby-Zombie",
		behavior::defaultFleeFromAfterNumAttacks(mob.MobParams, locator::Target(), random::randomIntInclusive(1, 2)),
		behavior::meleeAttack(mob),
		sequence("move-even-closer",
			isInRange(actor::Target(), 250, 400),
			behavior::moveTo(actor::Target(), Relative(1.8f))
		),
		behavior::chaseInOffensiveRange(mob, actor::Target(), Relative(1.8f)),
		behavior::chaseIfAttacked(mob, Relative(1.8f)),
		behavior::chaseIfWarned(mob, Relative(1.8f)),
		behavior::defaultRoam()
	),
	sequence("update-targets",
		every(0.5s,
			set(actor::Target(), actor::ClosestEnemy())
		),
		every(3s,
			set(location::Anchor(), location::Self())
		)
	));
}

}}}
