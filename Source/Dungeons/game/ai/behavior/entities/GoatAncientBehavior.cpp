#include "Dungeons.h"
#include "GoatAncientBehavior.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/MeleeAttackBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/provider/Move.h"
#include "game/ai/provider/Locations.h"
#include "game/ai/bt/decorator/BtDropFor.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/component/BehaviorOptionsComponent.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/provider/ActorStats.h"
#include "game/ai/behavior/FleeBehavior.h"
#include "game/ai/task/PlayAnimation.h"
#include "game/ai/condition/BtProbability.h"
#include "game/ai/bt/BtDelay.h"
#include "game/ai/condition/IsInRange.h"

namespace bt { namespace behavior { namespace entities {
	BehaviorTuple createGoatAncient(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
		const auto fleeHealthFraction = options.Get("flee-health-fraction", 0.4f);
		const auto fleeDuration = options.Get("flee-duration", 15.0s);

		return BehaviorTuple(
			selector("Goat",
				sequence("flee",
					predicate(lessThan(actorStats::healthFraction(), value(fleeHealthFraction))),
					behavior::defaultFleeFrom(actor::Target())
				),
				sequence("attack",
					predicate(actor::IsAlive(actor::Target())),
					behavior::meleeAttack(mob, actor::Target())
				),
				behavior::chase(actor::Target(), 2000.f, Relative(1)),
				behavior::defaultRoam()
			),
			parallel("update-targets",
				every(0.5s, set(actor::Target(), actor::ClosestEnemy(3000.f))),
				every(3s, sequence(
					isInRange(actor::Target(), mob.OffensiveRange),
					set(location::Anchor(), location::Self())
				))
			)
		);
	}
}}}
