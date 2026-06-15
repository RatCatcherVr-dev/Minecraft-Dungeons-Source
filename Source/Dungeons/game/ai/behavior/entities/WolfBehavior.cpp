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
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/condition/BtProbability.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/provider/BtRandom.h"
#include "game/ai/action/AnimActions.h"
#include "game/util/Tags.h"
#include "game/util/LocationQuery.h"

namespace bt { namespace behavior { namespace entities {

bool isPetTarget(AActor* actor) {	
	return actor->ActorHasTag(tags::petTarget);
}

BehaviorTuple createWolf(AMobCharacter& mob, const UBehaviorOptionsComponent& options, bool usePathFinding) {
	static Random rnd;

	const auto rightHand = rnd.nextBoolean() ? -1.f : 1.f;

	const auto disableAttacks = options.Get("DisableAttacks", 0.0f);
	const auto AttackTriggerDistance = options.Get("AttackTriggerDistance", 1000.f);
	const auto AttackEnterDistance = options.Get("AttackEnterDistance", 350.f);
	const auto AttackExitDistance = options.Get("AttackExitDistance", 500.f);
	const auto AttackChargeRange = options.Get("AttackChargeRange", 300.f);
	const auto AttackChargeMultiplier = options.Get("AttackChargeMultiplier", .9f);
	const auto ChaseMultiplier = options.Get("ChaseMultiplier", .6f);
	const auto FollowTriggerDistance = options.Get("FollowTriggerDistance", 1300.f);
	const auto AheadDistance = options.Get("AheadDistance", 400.f);
	const auto SideDistance = options.Get("SideDistance", 200.f);
	const auto RoamInterval = options.Get("RoamInterval", 3.s);

	const auto isPetEnemy = [&](AActor* actor) -> bool {
		const auto target = Cast<AMobCharacter>(actor);
		return target != nullptr && (isPetTarget(actor) || mob.IsHostileTowards(target)) && target->IsTargetable();
	};

	return BehaviorTuple(
		root(selector("Wolf",
			uninterruptible(sequence(
				startPredicate(locator::IsInRange(actor::Master(), { AttackTriggerDistance }) && isZero(value(disableAttacks)) ),
				selector(
					sequence("attack",
						isInRange(
							actor::Target(),
							AttackEnterDistance,
							AttackExitDistance
						),
						ifElse(
							isInRange(
								actor::Target(),
								AttackChargeRange
							),
							behavior::meleeAttack(mob),
							behavior::moveTo(move::withSettings(actor::Target(), move::defaultRequest().SetUsePathfinding(usePathFinding)), Relative(AttackChargeMultiplier))
						)
					),
					behavior::chaseInOffensiveRange(mob, move::withSettings(actor::Target(), move::defaultRequest().SetUsePathfinding(usePathFinding)), Relative(ChaseMultiplier))
				)
			)),
			uninterruptible(sequence(
				startPredicate(
					!locator::IsInRange(actor::Master(), { FollowTriggerDistance }) ||
					(
						!locator::IsInRange(actor::Master(), { AttackTriggerDistance }) &&
						lessThan(
							actor::CountInRange(actor::Master(), 1000.f, isPetEnemy),
							value(5)
						)
					)
				),
				follow(
					[
						rightHand,
						aheadDistance = AheadDistance,
						sideDistance = SideDistance
					](StateRef state) {
						if (const auto master = state.owner->GetMaster()) {
							if (locationquery::isInFrontOf(master, state.owner->GetActorLocation())) {
								return master->GetActorLocation() + master->GetActorForwardVector() * aheadDistance;
							} else {
								return master->GetActorLocation() + master->GetActorForwardVector().RotateAngleAxis(45.f * rightHand, FVector::UpVector) * sideDistance;
							}
						} else {
							return state.owner->GetActorLocation();
						}
					},
					move::defaultRequest().SetUsePathfinding(usePathFinding)
				),				
				onStop(
					set(
						location::Anchor(),
						[](StateRef state) { return state.owner->GetActorLocation() + state.owner->GetActorForwardVector() * 500.f; }
					)
				)
			)),
			parallel(
				behavior::smoothRoam(RoamInterval),
				sequence(
					every(1.s, selector(
						sequence(
							predicate(actor::IsInFront(actor::Master()) && random::randomBool(.9f)),
							anim::SetLookAtTarget(actor::Master())
						),
						anim::SetLookAtTarget(actor::Master())
					)),
					onStop(anim::ClearLookAtTarget())
				)
			)
		)),
		every(
			1s * rnd.nextFloat(.25f, .5f),
			set(
				actor::Target(),
				firstValue(
					actor::ClosestMob(2000.f, isPetTarget),
					actor::ClosestMob(1500.f, isPetEnemy)
				)
			)
		)
	);
}

}}}
