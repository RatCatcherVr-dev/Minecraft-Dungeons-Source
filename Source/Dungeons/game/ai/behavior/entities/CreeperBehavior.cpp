#include "Dungeons.h"
#include "CreeperBehavior.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/component/BehaviorOptionsComponent.h"
#include "game/ai/bt/BtLogic.h"
#include "game/util/Tags.h"
#include "game/ai/behavior/FleeBehavior.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/provider/ActorStats.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/task/attack/AoeAttack.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/action/HealthActions.h"
#include "game/component/HealthComponent.h"

namespace bt { namespace behavior { namespace entities {

	const auto isOcelot = [](AActor* actor) -> bool {
		const auto target = Cast<AMobCharacter>(actor);
		if (target) {
			if (target->EntityType == EntityType::Ocelot) {
				return true;
			}
		}

		return false;

	};

	BehaviorTuple createCreeper(AMobCharacter& mob, const UBehaviorOptionsComponent& options) { return BehaviorTuple(
		selector("Creeper",
			sequence(
				predicate(!isZero(ref(mob.MobParams.successfulAttacks.all))),
				health::Kill(actor::Self(), damageTag::explosion())
			),
			defaultFleeFrom(actor::ClosestMob(1600.0f, isOcelot)),
			greedySequence(
				isInRange(actor::Target(), options.Get("EnterAggroRange", 450), options.Get("ExitAggroRange", 600)),
				std::make_unique<UAoeAttack>(),
				moveTo(actor::Target(), Relative(1.0f))
			),
			chaseInOffensiveRange(mob, actor::Target(), Relative(0.5f)),
			chaseIfAttacked(mob, Relative(0.75f)),
			chaseIfWarned(mob, Relative(0.75f)),
			defaultRoam()
		),
		every(0.5s,
			set(actor::Target(), actor::ClosestEnemy())
		));
	}

}}}
