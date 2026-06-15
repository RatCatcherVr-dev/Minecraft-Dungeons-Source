#include "Dungeons.h"
#include "EvokerBehavior.h"
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
#include "game/ai/provider/Transform.h"
#include "game/ai/action/StateActions.h"

namespace bt { namespace behavior { namespace entities {

io::MobGroup createEvokerGroup() {
	return { {
		{ EntityType::Vex },
	} };
}

BehaviorTuple createEvoker(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
	return BehaviorTuple(root(selector("Evoker",
		sequence("flee-state",
			startCondition(isInRange(actor::Target(), options.Get("FleeRange", 300.f))),
			behavior::defaultFleeFromAfterNumAttacks(mob.MobParams, locator::Target(), value(1))
		),
		uninterruptible(sequence("summon-vexes",
			startPredicate(
				!actor::HasRecentlyAttacked(1s) &&
				locator::IsInRange(actor::Target(), options.Get("VexRange", 600.f))
			),
			make_unique<USummonTask>(
				options.Get("VexSummonCap", 4),
				createEvokerGroup(),
				options.Get("VexAnimationDuration", 2.4s),
				options.Get("VexSummonCooldown", 4s),
				bt::value(options.Get("VexSummonCountAtATime", 4)),
				bt::locator::RandomReachablePointAround(bt::location::Self(), options.Get("VexSummonRadius", 350.f))
			)
		)),
		uninterruptible(sequence("summon-fangs-front",
			startPredicate(
				!actor::HasRecentlyAttacked(2s) &&
				locator::IsInRange(actor::Target(), options.Get("FangLineTriggerRange", 1000.f)) &&
				(actor::IsBeingLookedAt(actor::Target(), PI * .25f) || actor::IsBeingLookedAway(actor::Target(), PI * .25f)) &&
				actor::IsInLineOfSight(actor::Target())
			),
			onStart(focus::Set(actor::Target(), EAIFocusPriority::Attacking)),
			onStop(focus::Clear(EAIFocusPriority::Attacking)),
			make_unique<USummonFormationTask>(
				options.Get("FangSequence"),
				options.Get("FangAnimationDuration", 2.4s),
				options.Get("FangSummonStart", 1.4s),
				options.Get("FangLineSummonCooldown", 2s),
				0.s,
				0.s,
				bt::formation::EvokerFangLine
			)
		)),
		uninterruptible(sequence("summon-fangs-around",
			startPredicate(
				!actor::HasRecentlyAttacked(2s) &&
				locator::IsInRange(actor::Target(), { 500.f, options.Get("FangCircleTriggerRange", 1000.f) })
			),
			onStart(focus::Set(actor::Target(), EAIFocusPriority::Attacking)),
			onStop(focus::Clear(EAIFocusPriority::Attacking)),
			make_unique<USummonFormationTask>(
				options.Get("FangSequence"),
				options.Get("FangAnimationDuration", 2.4s),
				options.Get("FangSummonStart", 1.4s),
				options.Get("FangCircleSummonCooldown", 6s),
				0.s,
				0.s,
				bt::formation::makeEvokerFangCircle(value(12), transform::distanceTo(actor::Target()))
			)
		)),
		sequence("chase",
			isInRange(actor::Target(), 2000),
			behavior::moveTo(actor::Target(), Relative(0.8f))
		),
		sequence("no-target",
			selector(
				behavior::chaseIfAttacked(mob, Relative(0.9f)),
				behavior::chaseIfWarned(mob, Relative(0.9f)),
				behavior::defaultRoam()
			)
		)
	)),
	every(0.5s,
		set(actor::Target(), actor::ClosestEnemy())
	)
); }

}}}
