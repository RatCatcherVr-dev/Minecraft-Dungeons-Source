#include "Dungeons.h"
#include "JackOLanternBehavior.h"
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
#include "game/ai/task/ReturnTask.h"
#include "game/ai/debug/BtDebug.h"
#include "game/component/TurnComponent.h"

namespace bt { namespace behavior { namespace entities {

BehaviorTuple createJackOLantern(class AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
	enum class State {
		Sleeping,
		Waking,
		Active,
		Melting,
		Dead
	};

	const auto stage = sharedRef(State::Sleeping);	

	return BehaviorTuple(
		selector("JackOLantern",
			root(selector(
				sequence(
					predicate(equals(stage, value(State::Sleeping))),
					parallel(
						playAnimation(options.Get("Sleeping"), true),
						sequence(
							predicate(actor::IsAnyPlayersInRange(options.Get("WakeupRange", 1000.f))),
							set(stage, value(State::Waking))
						)
					)
				),
				sequence(
					predicate(equals(stage, value(State::Waking))),
					debug::above("waking"),
					parallel(
						playAnimation(options.Get("Waking")),
						dropFor(options.Get("WakeupDuration", 2.7s), set(stage, value(State::Active)))
					)
				),
				sequence(
					predicate(equals(stage, value(State::Active))),
					debug::above("active"),
					parallel(
						uninterruptible(sequence(
							startPredicate(actor::IsAnyPlayersInFront()),
							make_unique<URangedAttack>(actor::Target())
						)),
						dropFor(options.Get("Lifetime", 10.s), set(stage, value(State::Melting)))
					)
				),
				sequence(
					predicate(equals(stage, value(State::Melting))),
					debug::above("melting"),
					parallel(
						playAnimation(options.Get("MeltAway")),
						dropFor(options.Get("MeltDuration", 10.s), set(stage, value(State::Dead)))
					)
				),
				sequence(
					startPredicate(equals(stage, value(State::Dead))),
					debug::above("dead"),
					playAnimation(options.Get("Dead"), true)
				)
			))
		),
		parallel("update-targets",
			every(0.5s, sequence(
				set(actor::Target(), actor::ClosestEnemy(options.Get("FocusRange", 3100.f)))
			))
		)
	);
}

}}}
