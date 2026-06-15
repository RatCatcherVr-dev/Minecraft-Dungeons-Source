#include "Dungeons.h"
#include "SlimeCauldronBehavior.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/FleeBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/task/attack/SummonTask.h"
#include "game/ai/task/attack/SummonFormationTask.h"
#include "game/ai/task/attack/formations/EvokerFormations.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/provider/Locators.h"
#include "game/ai/behavior/RangedAttackBehavior.h"
#include "game/ai/provider/ActorStats.h"
#include "game/ai/task/HealTask.h"
#include "game/ai/task/attack/RangedAttack.h"
#include "game/ai/task/attack/formations/CauldronFormations.h"
#include "game/ai/task/PlayAnimation.h"
#include "game/ai/action/CommonActions.h"
#include "game/ai/bt/decorator/BtDropFor.h"

namespace bt { namespace behavior { namespace entities {

BehaviorTuple createSlimeCauldron(class AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
	enum class State {
		Attacking,
		Melting
	};

	const auto stage = sharedRef(State::Attacking);

	return BehaviorTuple(
		selector(
			sequence(
				predicate(equals(stage, value(State::Attacking))),
				parallel(
					rangedAttackInOrientation(mob, actor::Target()),					
					dropFor(options.Get("Lifetime", 10.s), set(stage, value(State::Melting)))
				)
			),
			sequence(
				predicate(equals(stage, value(State::Melting))),
				selector(
					playAnimation(options.Get("MeltAway")),
					common::Apply(actor::Self(), [](AActor* mob) { mob->Destroy(); })
				)
			)
		),
		parallel("update-targets",
			every(0.5s, sequence(
				set(actor::Target(), actor::ClosestEnemy(3000.f)),
				focus::Set(actor::Target())
			))
		)
	);
}

}}}
