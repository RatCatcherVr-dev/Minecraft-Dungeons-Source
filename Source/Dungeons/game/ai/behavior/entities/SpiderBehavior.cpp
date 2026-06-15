#include "Dungeons.h"
#include "SpiderBehavior.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/RangedAttackBehavior.h"
#include "game/ai/behavior/MeleeAttackBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/behavior/FleeBehavior.h"
#include "game/ai/behavior/ChaseAndMeleeAttackBehavior.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/provider/Actors.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/util/Tags.h"
#include "game/ai/action/TransformActions.h"

namespace bt { namespace behavior { namespace entities {


	BehaviorTuple createSpider(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {

		const auto RangedAcquireTargetRangeUnits = options.Get("RangedAcquireTargetRangeUnits", 1000.f);
		const auto RangedLoseTargetRangeUnits = options.Get("RangedLoseTargetRangeUnits", 1200.f);

		return BehaviorTuple(
			selector("Spider",
				sequence(
					predicate(actor::HasTag(tags::inLove)),
					behavior::chaseAndMeleeAttack(mob, options)
				),

				every(0.5s,
					behavior::rangedAttack(mob, 650.0f, RangedAcquireTargetRangeUnits, RangedLoseTargetRangeUnits)
			),

			sequence("melee-attack",
				isInRange(actor::Target(), 700.f),
				predicate(actor::IsStuckInWeb(actor::Target())),
				selector(
					behavior::meleeAttack(mob),
					behavior::chargeTo(actor::Target(), Relative(1.7f), 1.5s, 5s, Relative(0.6f)),
					behavior::moveTo(actor::Target(), Relative(0.5f))
				)
			),
			every(1s,
				sequence("flee",			
					isInRange(actor::Target(), 700.f),
					predicate(!actor::IsStuckInWeb(actor::Target())),
					behavior::defaultFleeFrom(locator::Target())
				)
			),
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
	);
}

}}}
