#include "Dungeons.h"
#include "WraithBehavior.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/action/TransformActions.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/provider/Actors.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/task/attack/SummonFormationTask.h"
#include "game/ai/task/attack/formations/WraithFormations.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/task/move/TeleportTask.h"
#include "game/ai/provider/Locators.h"
#include "game/ai/action/AnimActions.h"
#include "game/ai/bt/decorator/BtEvery.h"

namespace bt { namespace behavior { namespace entities {

BehaviorTuple createWraith(AMobCharacter& mob, const UBehaviorOptionsComponent& options) { return BehaviorTuple(
	root(selector(
		sequence(
			predicate(!locator::IsInRange(actor::Target(), options.Get("MoveTowardsTriggerRange", 1200.f))),
			behavior::moveTo(actor::Target(), Relative(options.Get("MoveTowardsMultiplier", 2.f)))
		),

		uninterruptible(sequence(
			startPredicate(
				locator::IsInRange(actor::Target(), options.Get("AttackTriggerRange", 900.f)) &&
				!actor::HasRecentlyAttacked(2.s)
			),
			sequence("attack-formation",
				onStart(transform::LookAtYaw(actor::Target())),
				make_unique<USummonFormationTask>(
					options.Get("IceCloudSequence"),
					options.Get("IceCloudAnimationDuration", 4.s),
					options.Get("IceCloudSummonDelay", 4.s),
					options.Get("IceCloudSummonCooldown", 5.s),
					0.s,
					0.s,
					formation::makeIceRandom(						
						options.GetClass("CenterClass"),
						options.GetClass("OuterClass"),
						options.Get("IceCloudPatternIndex", 1),
						options.Get("IceCloudUnitSize", 100.f),
						options.Get("IceCloudSpreadSpeed", .1s)
					),
					USummonFormationTask::SummonActor,
					options.Get("IceCloudEmergeDuration", .1s)
				)
			)
		)),

		uninterruptible(sequence(
			startPredicate(locator::IsInRange(actor::Target(), options.Get("TeleportAwayTriggerRange", 300.f))),
			sequence(
				onStart(anim::ClearLookAtTarget()),
				make_unique<UTeleportTask>(
					options.Get("TeleportOutDuration", 2.s),
					options.Get("TeleportInDuration", 2.s),
					value(0.s),
					locator::RandomReachablePointAround(
						actor::Target(),
						options.Get("TeleportAwayDistance", 900.f) - 200.f,
						options.Get("TeleportAwayDistance", 900.f)
					),
					actor::Target()
				)
			)
		)),

		uninterruptible(sequence(
			onStart(anim::SetLookAtTarget(actor::Target())),
			predicate(!locator::IsInRange(actor::Target(), options.Get("TeleportAwayTriggerRange", 300.f))),
			behavior::moveTo(
				move::From(
					locator::RandomReachablePointAround(
						locator::Side(actor::Target(), options.Get("MoveAroundRadius", 600.f), true),
						200.f
					)
				),
				Relative(options.Get("MoveAroundMultiplier", 1.3f))
			)
		)),

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
