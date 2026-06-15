#include "Dungeons.h"
#include "FrozenZombieBehavior.h"
#include "game/ai/action/CommonActions.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/MeleeAttackBehavior.h"
#include "game/ai/behavior/RangedAttackBehavior.h"
#include "game/ai/behavior/ChaseAndMeleeAttackBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/provider/Locators.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/condition/AttackPredicates.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/condition/BtProbability.h"
#include "game/component/BehaviorOptionsComponent.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/util/Tags.h"
#include "game/ai/action/TransformActions.h"
#include "game/ai/condition/IsInRange.h"
#include "util/Random.h"

namespace bt { namespace behavior { namespace entities {
	BehaviorTuple createFrozenZombie(AMobCharacter& mob, const UBehaviorOptionsComponent& options) { return BehaviorTuple(
		selector("FrozenZombie",

			rangedAttack(
				mob,
				options.Get("MinAttackDistance", 100.f),
				options.Get("RangedAcquireTargetRangeUnits", 1000.f),
				options.Get("RangedLoseTargetRangeUnits", 1200.f),
				actor::Target()
			),

			sequence("melee-attack",
				isInRange(actor::Target(), options.Get("MinAttackDistance", 700.0f)),
				predicate(actor::IsFreezing(actor::Target())),
				selector(
					behavior::meleeAttack(mob),
					behavior::chargeTo(actor::Target(), Relative(0.6f), 1.5s, 5s, Relative(0.6f)),
					behavior::moveTo(actor::Target(), Relative(0.5f))
				)
			),

			behavior::meleeAttack(mob),

			every(1.21s,
				behavior::chaseInOffensiveRange(mob, actor::Target(), Relative(0.4f))
			),

			behavior::chaseIfAttacked(mob, Relative(0.5f)),
			behavior::chaseIfWarned(mob, Relative(0.5f)),
			behavior::defaultRoam()
		),
		sequence("update-targets",
			every(0.5s,
				set(actor::Target(), actor::ClosestEnemy())
			),
			every(3s,
				set(location::Anchor(), location::Self())
			)
		)
	);}
}}}
