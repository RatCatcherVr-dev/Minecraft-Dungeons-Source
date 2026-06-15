#include "Dungeons.h"
#include "LlamaBehavior.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/MeleeAttackBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/behavior/RangedAttackBehavior.h"
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
#include "game/component/RangedAttackComponent.h"

namespace bt { namespace behavior { namespace entities {

bool isPetTarget_(AActor* actor) {	
	return actor->ActorHasTag(tags::petTarget);
}

BehaviorTuple createLlama(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
	static Random rnd;

	const auto rightHand = rnd.nextBoolean() ? -1.f : 1.f;

	const auto isPetEnemy = [&](AActor* actor) -> bool {
		const auto target = Cast<AMobCharacter>(actor);
		return target != nullptr && (isPetTarget_(actor) || mob.IsHostileTowards(target)) && target->IsTargetable();
	};

	URangedAttackComponent& rangedComponent = *mob.FindComponentByClass<URangedAttackComponent>();

	return BehaviorTuple(
		root(selector("Llama",
			uninterruptible(sequence(
				startPredicate(locator::IsInRange(actor::Master(), { options.Get("AttackTriggerDistance", 1500.f) })),
				selector(
					rangedAttack(
						mob,
						options.Get("MinAttackDistance", 0.f),
						options.Get("RangedAcquireTargetRangeUnits", 1000.f),
						options.Get("RangedLoseTargetRangeUnits", 1500.f),
						actor::Target()
					),
					behavior::chase(actor::Target(), FloatRange(rangedComponent.GetAttackRange(), options.Get("AttackTriggerDistance", 1500.f)), Relative(options.Get("ChaseMultiplier", .6f)))
				)
			)),
			uninterruptible(sequence(
				startPredicate(
					!locator::IsInRange(actor::Master(), { options.Get("FollowTriggerDistance", 1300.f) }) ||
					(
						!locator::IsInRange(actor::Master(), { options.Get("AttackTriggerDistance", 1000.f) }) &&
						lessThan(
							actor::CountInRange(actor::Master(), 1000.f, isPetEnemy),
							value(5)
						)
					)
				),
				follow(
					[
						rightHand,
						aheadDistance = options.Get("AheadDistance", 400.f),
						sideDistance = options.Get("SideDistance", 200.f)
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
					}
				),				
				onStop(
					set(
						location::Anchor(),
						[](StateRef state) { return state.owner->GetActorLocation() + state.owner->GetActorForwardVector() * 500.f; }
					)
				)
			)),
			parallel(
				behavior::smoothRoam(options.Get("RoamInterval", 3.s)),
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
					actor::ClosestMob(2000.f, isPetTarget_),
					actor::ClosestMob(1500.f, isPetEnemy)
				)
			)
		)
	);
}

bt::BehaviorTuple createLlamaMob(class AMobCharacter& mob, const UBehaviorOptionsComponent& options)
{
	const auto aggressionDuration = bt::Seconds(options.Get("agressionDuration", 15.f));
	return BehaviorTuple(
		selector("LlamaMob",
			sequence(
				predicate(actor::IsRecentlyDamagedFromAttack(aggressionDuration)),
				predicate(actor::IsAlive(actor::LastAttacker())),
				predicate(actor::IsVisible(actor::LastAttacker())),
				behavior::rangedAttackAndMoveCloser(mob, 0, options.Get("RangedAcquireTargetRangeUnits", 400.f), options.Get("RangedLoseTargetRangeUnits", 1200.f), options.Get("RangedAcquireTargetRangeUnits", 1000.f)*0.8f, actor::LastAttacker(), Relative(0.8f), 20)
			),
			behavior::defaultRoam()
		)
	);
}

}}}
