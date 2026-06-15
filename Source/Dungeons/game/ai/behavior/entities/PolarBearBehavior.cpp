#include "Dungeons.h"
#include "PolarBearBehavior.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/MeleeAttackBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/provider/Move.h"
#include "game/ai/provider/Locations.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/component/BehaviorOptionsComponent.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/condition/BtPredicate.h"

namespace bt { namespace behavior { namespace entities {
	BehaviorTuple createPolarBear(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
		const auto aggressionDuration = bt::Seconds(options.Get("agressionDuration", 15.f));
		return BehaviorTuple(
			selector("polar-bear",
				sequence(
					predicate(actor::IsRecentlyDamagedFromAttack(aggressionDuration)),
					predicate(actor::IsAlive(actor::LastAttacker())),
					predicate(actor::IsVisible(actor::LastAttacker())),
					behavior::meleeAttack(mob, actor::LastAttacker())
				),
				sequence(
					predicate(actor::IsVisible(actor::LastAttacker())),
					behavior::chaseIfAttackedForDuration(mob, Relative(1.0f), aggressionDuration)
				),
				behavior::defaultRoam()
			)
		);
	}
}}}
