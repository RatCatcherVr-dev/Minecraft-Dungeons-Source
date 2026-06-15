#include "Dungeons.h"
#include "SkeletonBehavior.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/component/RangedAttackComponent.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/FleeBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/RangedAttackBehavior.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/provider/Locators.h"
#include "util/FloatRange.h"
#include "game/component/BehaviorOptionsComponent.h"

namespace bt { namespace behavior { namespace entities {

BehaviorTuple createSkeleton(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
	URangedAttackComponent& rangedComponent = *mob.FindComponentByClass<URangedAttackComponent>();

	const auto RangedMinDistanceBeforeFleeing = options.Get("RangedMinDistanceBeforeFleeing", 700.f);
	const auto FleeAfterNumAttacks = options.Get("FleeAfterNumAttacks", 1);
	const auto MinAttackDistance = options.Get("MinAttackDistance", 10.f);
	const auto RangedAcquireTargetRangeUnits = options.Get("RangedAcquireTargetRangeUnits", 1000.f);
	const auto RangedLoseTargetRangeUnits = options.Get("RangedLoseTargetRangeUnits", 1200.f);
	const auto ChaseSpeedMultiplier = options.Get("ChaseSpeedMultiplier", .5f);
	const auto ChaseIfAttackedSpeedMultiplier = options.Get("ChaseIfAttackedSpeedMultiplier", .6f);
	const auto ChaseIfWarnedSpeedMultiplier = options.Get("ChaseIfWarnedSpeedMultiplier", .6f);

	return BehaviorTuple(
		selector("Skeleton",
			sequence("flee-state",
				startCondition(isInRange(actor::Target(), RangedMinDistanceBeforeFleeing)),
				behavior::defaultFleeFromAfterNumAttacks(mob.MobParams, locator::Target(), value(FleeAfterNumAttacks))
			),
			rangedAttack(
				mob,
				MinAttackDistance,
				RangedAcquireTargetRangeUnits,
				RangedLoseTargetRangeUnits,
				actor::Target()
			),
			behavior::chase(actor::Target(), FloatRange(rangedComponent.GetAttackRange(), RangedAcquireTargetRangeUnits), Relative(ChaseSpeedMultiplier)),
			behavior::chaseIfAttacked(mob, Relative(ChaseIfAttackedSpeedMultiplier)),
			behavior::chaseIfWarned(mob, Relative(ChaseIfWarnedSpeedMultiplier)),
			behavior::defaultRoam()
		),
		parallel("update-targets",
			every(.5s, set(actor::Target(), actor::ClosestEnemy())),
			every(3s, sequence(
				isInRange(actor::Target(), mob.OffensiveRange),
				set(location::Anchor(), location::Self())
			))
		)
	);
}

}}}
