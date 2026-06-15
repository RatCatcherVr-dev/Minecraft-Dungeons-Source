#include "Dungeons.h"
#include "WitchBehavior.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/RangedAttackBehavior.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/task/attack/SummonFormationTask.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/provider/Locators.h"
#include "game/ai/provider/ActorStats.h"
#include "game/ai/task/HealTask.h"
#include "game/ai/task/attack/RangedAttack.h"

namespace bt { namespace behavior { namespace entities {

BehaviorTuple createWitch(class AMobCharacter& mob, const UBehaviorOptionsComponent& options) {	return BehaviorTuple(
	root(selector(
		uninterruptible(sequence(
			startPredicate(lessThan(actorStats::healthFraction(), value(.8f))),
			make_unique<UHealTask>(
				options.Get("HealSequence"),
				options.Get("HealAnimationDuration", 1.s),
				options.Get("HealDelay", .6s),
				options.Get("HealCooldown", 5.s),
				options.Get("HealAmount", 50.f)
			)
		)),

		uninterruptible(sequence(
			startPredicate(
				locator::IsInRange(actor::Target(), options.Get("AttackTriggerRange", 600.f)) &&
				!actor::HasRecentlyAttacked(2.s)
			),
			behavior::moveTo(actor::Target(), Relative(.01f)),
			rangedAttackInOrientation(mob, actor::Target())			
		)),

		sequence(
			predicate(!locator::IsInRange(actor::Target(), options.Get("MoveTowardsTriggerRange", 600.f))),
			behavior::moveTo(actor::Target(), Relative(options.Get("CloseInMultiplier", .9f)))
		),

		uninterruptible(
			behavior::moveTo(
				move::From(
					locator::RandomReachablePointAround(
						locator::Side(actor::Target(), options.Get("MoveAroundRadius", 600.f), true),
						200.f
					)
				),
				Relative(options.Get("MoveAroundMultiplier", 1.3f))
			)
		),

		behavior::defaultRoam()
	)),
	parallel("update-targets",
		every(0.5s, set(actor::Target(), actor::ClosestEnemy(3000.f))),
		every(3s, sequence(
			isInRange(actor::Target(), mob.OffensiveRange),
			set(location::Anchor(), location::Self())
		))
	)
); }

}}}
