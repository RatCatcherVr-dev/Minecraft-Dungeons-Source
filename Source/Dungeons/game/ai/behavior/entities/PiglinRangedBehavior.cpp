#include "Dungeons.h"
#include "PiglinRangedBehavior.h"
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
#include "game/ai/task/PlayAnimation.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/action/FocusActions.h"

namespace bt { namespace behavior { namespace entities {

BehaviorTuple createPiglinRanged(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
	const auto isShooting = sharedRef(false);

	URangedAttackComponent& rangedComponent = *mob.FindComponentByClass<URangedAttackComponent>();

	return BehaviorTuple(
		selector("PiglinHunter",
			sequence("flee-state",
				startCondition(isInRange(actor::Target(), options.Get("RangedMinDistanceBeforeFleeing", 700.f))),
				set(isShooting, value(false)),
				behavior::defaultFleeFromAfterNumAttacks(mob.MobParams, locator::Target(), value(options.Get("FleeAfterNumAttacks", 3)), 1000.f, Relative(options.Get("FleeSpeedMultiplier", 2)))
			),
			sequence(
				predicate(equals(isShooting, value(false))),
				chain(
					sequence(
						focus::Set(actor::Target(), EAIFocusPriority::Attacking),
						playAnimation(options.Get("PullUpAnimation"))
					),
					set(isShooting, value(true))
				)
			),
			sequence(
				predicate(equals(isShooting, value(true))),
				rangedAttack(
					mob,
					options.Get("MinAttackDistance", 10.f),
					options.Get("RangedAcquireTargetRange", 1000.f),
					options.Get("RangedLoseTargetRange", 1200.f),
					actor::Target()
				)
			),
			behavior::chase(actor::Target(), FloatRange(rangedComponent.GetAttackRange(), options.Get("RangedAcquireTargetRange", 1000.f)), Relative(options.Get("ChaseSpeedMultiplier", 2.f))),
			behavior::chaseIfAttacked(mob, Relative(options.Get("ChaseIfAttackedSpeedMultiplier", 2.f))),
			behavior::chaseIfWarned(mob, Relative(options.Get("ChaseIfWarnedSpeedMultiplier", 2.f))),
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
