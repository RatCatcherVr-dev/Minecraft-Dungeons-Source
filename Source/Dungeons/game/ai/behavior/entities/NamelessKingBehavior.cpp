#include "Dungeons.h"
#include "NamelessKingBehavior.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/FleeBehavior.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/task/attack/SummonFormationTask.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/task/attack/MirrorTask.h"
#include "game/ai/task/attack/formations/NamelessKingFormations.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/task/TurnTask.h"
#include "game/ai/behavior/RangedAttackBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"

namespace bt { namespace behavior { namespace entities {

bool isRoyalGuard(AActor* actor) {
	if (const auto mobCharacter = Cast<AMobCharacter>(actor)) {
		return mobCharacter->EntityType == EntityType::SkeletonVanguard;
	}

	return false;
}

BehaviorTuple createNamelessKing(class AMobCharacter& mob, const UBehaviorOptionsComponent& options) {	
	const auto justSummoned = sharedRef(false);

	return BehaviorTuple(
		root(selector("NamelessKing",
			uninterruptible(sequence("mirror",
				startPredicate(
					(justSummoned || !actor::HasRecentlyAttacked(options.Get("MirrorCooldown", 4s))) &&
					actor::IsRecentlyDamagedFromAttack(3s)
				),
				make_unique<UMirrorTask>(
					options.Get("TeleportOutDuration", 2s),
					options.Get("TeleportInDuration", 2s),
					options.Get("MirrorCount", 8),
					options.Get("CircleRadiusMax", 1500.f),
					options.Get("AnchorDistanceMax", 1300.f),
					options.Get("MirrorDistanceMax", 2300.f),
					actor::Target(),
					location::StartPos()
				),
				onStop(set(justSummoned, value(false)))
			)),
			uninterruptible(sequence("summon-if-close-enough",
				startPredicate(
					locator::IsInRange(actor::Target(), 1400) &&
					lessThan(
						actor::CountInRange(actor::Self(), 3000.f, isRoyalGuard),
						value(options.Get("GuardLeftoverCount", 3))
					)
				),
				alwaysTrue(make_unique<UTurnTask>(actor::Target(), 300.f)),
				make_unique<USummonFormationTask>(
					options.Get("GuardSummonSequence"),
					options.Get("GuardSummonAnimationDuration", 2.4s),
					options.Get("GuardSummonStart", 1.4s),
					options.Get("GuardSummonCooldown", 6s),
					0.s,
					0.s,
					formation::NamelssKingDefensive(
						options.Get("GuardSummonHalfCount", 3),
						options.Get("GuardSummonSpacing", 140.f)
					)
				),
				onStop(set(justSummoned, value(true)))
			)),
			uninterruptible(sequence(
				startPredicate(
					locator::IsInRange(actor::Target(), options.Get("RangedTriggerRange", 1500.f))
				),
				behavior::moveTo(actor::Target(), Relative(.01f)),
				rangedAttackInOrientation(mob, actor::Target())
			)),
			defaultFleeFrom(actor::Target(), 1000.f),
			sequence("face the target",
				predicate(!locator::IsInRange(actor::Target(), { 500.f })),
				make_unique<UTurnTask>(actor::Target(), 300.f)
			)
		)),
		every(0.5s,
			set(actor::Target(), actor::ClosestPlayer())
		)
	);
}

}}}
