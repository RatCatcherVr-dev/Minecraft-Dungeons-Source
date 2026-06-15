#include "Dungeons.h"
#include "WolfBehavior.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/MeleeAttackBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/provider/Actors.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/action/CommonActions.h"
#include "game/ai/condition/BtProbability.h"
#include "game/ai/provider/Locators.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/bt/decorator/BtDropFor.h"
#include "game/ai/action/TransformActions.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/provider/BtRandom.h"
#include "game/ai/action/AnimActions.h"
#include "game/ai/task/attack/SummonTask.h"
#include "game/ai/task/attack/formations/HorsemenFormations.h"
#include "game/ai/action/HealthActions.h"
#include "game/ai/task/PlayAnimation.h"
#include "game/util/Tags.h"
#include "game/component/HealthComponent.h"

namespace bt { namespace behavior { namespace entities {

BehaviorTuple createOrdinaryHorse(AMobCharacter& mob, const UBehaviorOptionsComponent& options) { return BehaviorTuple(
	root(selector(
		uninterruptible(sequence(
			startPredicate(locator::IsInRange(actor::Target(), options.Get("SummonTriggerRange", 1200))),
			selector(
				make_unique<USummonFormationTask>(
					options.Get("HorsemenSummonSequence"),
					options.Get("HorsemenSummonAnimationDuration", 4.s),
					options.Get("HorsemenSummonDelay", 4.s),
					options.Get("HorsemenSummonCooldown", 5.s),
					0.s,
					0.s,
					formation::makeHorsemenSquare(
						options.Get("HorsemenRadiusBase", 200),
						options.Get("HorsemenRadiusIncrement", 200)
					),
					USummonFormationTask::SummonMobWithDrops,
					options.Get("EmergeDuration", .1s)
				),
				health::Kill(actor::Self(), damageTag::def(), 1000.f)
			)
		)),
		sequence(
			predicate(locator::IsInRange(
				actor::Target(),
				{ options.Get("SummonTriggerRange", 1200), options.Get("HorseCrazyRange", 1800) }
			)),
			common::Exec([] { UE_LOG(LogTemp, Warning, TEXT("ok")); }),
			playAnimation(options.Get("HorseCrazySequence"), true)
		),
		playAnimation(options.Get("IdleSequence"), true)
	)),
	every(.5s, set(actor::Target(), actor::ClosestPlayer()))
); }

}}}
