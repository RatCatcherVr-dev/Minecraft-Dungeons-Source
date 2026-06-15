#include "Dungeons.h"
#include "game/ai/bt/BtLogic.h"
#include "AnimalBehavior.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/FleeBehavior.h"
#include "game/ai/behavior/MeleeAttackBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/provider/BtRandom.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/condition/BtPredicate.h"
#include "util/Random.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/component/BehaviorOptionsComponent.h"
#include "OcelotBehavior.h"


//D11.CH - START
namespace bt { namespace behavior { namespace entities {

	BehaviorTuple createOcelot(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {

		static Random rnd;

		const auto isOcelotEnemy = [&](AActor* actor) -> bool {
			const auto target = Cast<AMobCharacter>(actor);
			if (target) {
				if (target->EntityType == EntityType::Creeper) {
					return true;
				}
			}

			return false;

		};

		return BehaviorTuple(
			selector("Ocelot",

				defaultFleeFrom(actor::ClosestPlayer(), 500.f),

				sequence("flee-condition",
					predicate(actor::IsRecentlyDamagedFromAttack(10s)),
					behavior::defaultFleeFrom(actor::LastAttacker())
				),

				behavior::chaseInOffensiveRange(mob, actor::Target(), Relative(1.8f)),

				behavior::meleeAttack(mob),

				behavior::defaultRoam()

			),
			
			sequence("update-targets",
				parallel(
					every(0.5s,
						set(actor::Target(), actor::ClosestMob(2000.f, isOcelotEnemy))
					),
					every(3s,
						set(location::Anchor(), location::Self())
					)
				)
			)
		);
	}
}}}


