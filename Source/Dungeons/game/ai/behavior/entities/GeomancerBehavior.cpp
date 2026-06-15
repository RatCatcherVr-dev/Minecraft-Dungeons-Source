#include "Dungeons.h"
#include "GeomancerBehavior.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/behavior/FleeBehavior.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/task/attack/formations/GeomancerFormations.h"
#include "game/ai/task/attack/SummonFormationTask.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/task/TurnTask.h"
#include "game/util/Tags.h"
#include "game/component/BehaviorOptionsComponent.h"

namespace bt { namespace behavior { namespace entities {

bool isHostileMob(AActor* actor) {
	const auto mobCharacter = Cast<AMobCharacter>(actor);

	return mobCharacter &&
		mobCharacter->EntityType != EntityType::Geomancer &&
		mobCharacter->EntityType != EntityType::GeomancerWall &&
		mobCharacter->EntityType != EntityType::GeomancerBomb &&
		!mobCharacter->ActorHasTag(tags::inLove) &&
		mobCharacter->IsTargetable();
}

BehaviorTuple createGeomancer(AMobCharacter& mob, const UBehaviorOptionsComponent& options) { return BehaviorTuple(
	root(selector("Geomancer",
		uninterruptible(sequence("block-state",
			startPredicate(
				!actor::HasRecentlyAttacked(options.Get("AttackedCooldown", 2.s)) &&
				locator::IsInRange(actor::Target(), 700) &&
				actor::IsBeingLookedAt(actor::Target(), PI * .2f)
			),
			behavior::facing(
				make_unique<USummonFormationTask>(
					options.Get("WallSequence"),
					1.s,
					.5s,
					options.Get("WallCooldown", 2.s),
					0.s,
					0.s,
					formation::GeomancerWallLine,
					USummonFormationTask::SummonMobNoAdjustPosition
				)
			)
		)),
		sequence("flee-state",
			startCondition(isInRange(actor::Target(), 700)),
			behavior::defaultFleeFrom(locator::Target())
		),
		uninterruptible(sequence("full-offensive-state",
			startPredicate(
				!actor::HasRecentlyAttacked(options.Get("AttackedCooldown", 2s)) &&
				locator::IsInRange(actor::Target(), 2000) &&
				lessThan(actor::CountInRange(actor::Self(), 3000.f, isHostileMob), value(3))
			),
			behavior::facing(
				make_unique<USummonFormationTask>(
					options.Get("BombManySequence"),
					1.s,
					.5s,
					options.Get("BombManyCooldown", 2.s),
					0.s,
					0.s,
					formation::GeomancerBombMultiple,
					USummonFormationTask::SummonMobNoAdjustPosition
				)
			)
		)),
		uninterruptible(sequence("mildly-offensive-state",
			startPredicate(
				!actor::HasRecentlyAttacked(options.Get("AttackedCooldown", 2s)) &&
				locator::IsInRange(actor::Target(), FloatRange(900, 2000))
			),
			behavior::facing(
				make_unique<USummonFormationTask>(
					options.Get("BombSoloSequence"),
					1.s,
					.5s,
					options.Get("BombSoloCooldown", 2.s),
					0.s,
					0.s,
					formation::GeomancerBombSolo,
					USummonFormationTask::SummonMobNoAdjustPosition
				)
			)
		)),
		uninterruptible(sequence("random-block-state",
			startPredicate(
				!actor::HasRecentlyAttacked(options.Get("AttackedCooldown", 2s)) &&
				locator::IsInRange(actor::Target(), 2000)
			),
			behavior::facing(
				make_unique<USummonFormationTask>(
					options.Get("RandomSequence"),
					1.s,
					.5s,
					options.Get("RandomCooldown", 2.s),
					0.s,
					0.s,
					formation::GeomancerAll,
					USummonFormationTask::SummonMobNoAdjustPosition
				)
			)
		)),
		behavior::defaultRoam()		
	)),
	parallel(
		every(0.5s, set(actor::Target(), actor::ClosestEnemy())),
		every(3s, set(location::Anchor(), location::Self()))
	)
); }

}}}