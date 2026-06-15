#include "Dungeons.h"
#include "GoatBehavior.h"
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

namespace bt { namespace behavior { namespace entities {
	BehaviorTuple createGoat(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
		const auto aggressionDuration = bt::Seconds(options.Get("aggression-duration", 10.0f));
		const auto fleeHealthFraction = options.Get("flee-health-fraction", 0.4f);
		const auto fleeDuration = options.Get("flee-duration", 15.0s);

		return BehaviorTuple(
			selector("Goat",
				sequence("flee",
					predicate(actor::IsRecentlyDamagedFromAttack(aggressionDuration)),
					predicate(lessThan(actorStats::healthFraction(), value(fleeHealthFraction))),
					behavior::defaultFleeFrom(actor::LastAttacker())
				),
				sequence("attack",
					predicate(actor::IsRecentlyDamagedFromAttack(aggressionDuration)),
					predicate(actor::IsAlive(actor::LastAttacker())),
					behavior::meleeAttack(mob, actor::LastAttacker())
				),				
				behavior::chaseIfAttackedForDuration(mob, Relative(1.0f), aggressionDuration),
				behavior::defaultRoam()
			)
		);
	}
}}}