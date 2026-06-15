#include "Dungeons.h"
#include "BlazeBehavior.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/BlazeFire.h"
#include "game/component/RangedAttackComponent.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/FleeBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/RangedAttackBehavior.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/provider/Locators.h"
#include "game/ai/provider/ActorStats.h"
#include "util/FloatRange.h"
#include "game/component/BehaviorOptionsComponent.h"

namespace bt { namespace behavior { namespace entities {

BehaviorTuple createBlaze(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
	URangedAttackComponent& rangedComponent = *mob.FindComponentByClass<URangedAttackComponent>();

	const auto getClosestFire = [](StateRef state) -> FLocator {
		float distance;
		return actorquery::getClosestActor(state.owner, 1500.f, ABlazeFire::StaticClass(), distance);
	};

	const auto closestFire = sharedRef<FLocator>({});

	return BehaviorTuple(
		selector("Skeleton",
			sequence(
				predicate(lessThan(actorStats::healthFraction(), value(options.Get("HealthFractionThreshold", .5f)))),
				selector(
					sequence(
						predicate([=](StateRef) { return closestFire->IsValid(); }),
						behavior::moveTo([=](StateRef) { return closestFire->GetLocationLike().GetValue(); }, Relative(1.f))
					),
					sequence(
						startCondition(isInRange(actor::Target(), options.Get("LowHealthRangedMinDistanceBeforeFleeing", 900.f))),
						behavior::defaultFleeFromAfterNumAttacks(mob.MobParams, locator::Target(), value(options.Get("LowHealthFleeAfterNumAttacks", 1)))
					),
					rangedAttack(
						mob,
						options.Get("LowHealthMinAttackDistance", 10.f),
						options.Get("LowHealthRangedAcquireTargetRangeUnits", 1000.f),
						options.Get("LowHealthRangedLoseTargetRangeUnits", 1200.f),
						actor::Target()
					),
					behavior::defaultRoam()
				)
			),
			sequence(
				predicate(greaterThanOrEquals(actorStats::healthFraction(), value(options.Get("HealthFractionThreshold", .5f)))),
				selector(
					sequence(
						startCondition(isInRange(actor::Target(), options.Get("RangedMinDistanceBeforeFleeing", 700.f))),
						behavior::defaultFleeFromAfterNumAttacks(mob.MobParams, locator::Target(), value(options.Get("FleeAfterNumAttacks", 2)))
					),
					rangedAttack(
						mob,
						options.Get("MinAttackDistance", 10.f),
						options.Get("RangedAcquireTargetRangeUnits", 1000.f),
						options.Get("RangedLoseTargetRangeUnits", 1200.f),
						actor::Target()
					)
				)
			),
			behavior::chase(actor::Target(), FloatRange(rangedComponent.GetAttackRange(), options.Get("RangedAcquireTargetRangeUnits", 1000.f)), Relative(options.Get("ChaseSpeedMultiplier", .5f))),
			behavior::chaseIfAttacked(mob, Relative(options.Get("ChaseIfAttackedSpeedMultiplier", .6f))),
			behavior::chaseIfWarned(mob, Relative(options.Get("ChaseIfWarnedSpeedMultiplier", .6f))),
			behavior::defaultRoam()
		),
		parallel("update-targets",
			every(.5s, set(actor::Target(), actor::ClosestEnemy())),
			every(.5s, sequence(
				predicate(lessThan(actorStats::healthFraction(), value(.5f))),
				set(closestFire, getClosestFire)
			)),
			every(3s, sequence(
				isInRange(actor::Target(), mob.OffensiveRange),
				set(location::Anchor(), location::Self())
			))
		)
	);
}

}}}
