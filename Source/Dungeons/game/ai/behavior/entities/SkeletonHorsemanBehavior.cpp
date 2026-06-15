#include "Dungeons.h"
#include "SkeletonHorsemanBehavior.h"
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
#include "game/ai/action/CommonActions.h"
#include "game/ai/condition/BtProbability.h"
#include "game/ai/provider/Locators.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/bt/decorator/BtDropFor.h"
#include "game/ai/action/TransformActions.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/behavior/RangedAttackBehavior.h"
#include "game/ai/provider/BtRandom.h"
#include "game/ai/action/AnimActions.h"
#include "game/ai/task/attack/RangedAttack.h"
#include "game/ai/task/PlayAnimation.h"
#include "game/actor/HorsemenTargetProvider.h"
#include "game/ai/action/StateActions.h"

namespace bt { namespace behavior { namespace entities {

BehaviorTuple createSkeletonHorseman(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
	enum class State {
		Spawning,
		Orbiting,
		Attacking,
		Retreating
	};

	const auto stage = sharedRef(State::Spawning);
	
	const auto sign = sharedRef(1.f);

	const auto horsemenTargetProvider = sharedObjectRef<AHorsemenTargetProvider>();

	static auto counter = 0;
	counter = (counter + 1) % 4;

	const auto orbit = [
		=, 
		rotateRadius = options.Get("RotateRadius", 400.f) + counter * options.Get("RadiusSpacing", 150.f)
	](StateRef state) {
		if (state.params().target.HasActor()) {
			const auto target = static_cast<AActor*>(state.params().target);
			const auto targetLocation = target->GetActorLocation();
		
			const auto towardsInRadius = (targetLocation - state.owner->GetActorLocation()).GetSafeNormal() * rotateRadius;
			const auto sideInRadius = towardsInRadius.RotateAngleAxis(90.f * sign(state), FVector::UpVector);

			return targetLocation - towardsInRadius + sideInRadius;
		} else {
			return state.owner->GetActorLocation();
		}
	};

	const bt::Provider<AActor*> mostAggresive = [=](StateRef state) -> AActor* {
		if (horsemenTargetProvider) {
			return horsemenTargetProvider->GetMostAggresive();
		} else {
			horsemenTargetProvider(state, actorquery::getFirstActor<AHorsemenTargetProvider>(&state.world()));
			return nullptr;
		}
	};

	return BehaviorTuple(
		selector(
			sequence(
				predicate(equals(stage, value(State::Spawning))),
				playAnimation(options.Get("Spawn")),
				alwaysTrue(dropFor(options.Get("SpawnDuration", 0.867s),
					parallel(
						set(stage, value(State::Orbiting))
					)
				))
			),
			sequence(
				predicate(!equals(stage, value(State::Spawning))),
				root(selector("SkeletonHorseman",
					uninterruptible(sequence(
						startPredicate(equals(stage, value(State::Attacking))),
						sequence(							
							rangedAttackInOrientation(mob, actor::Target()),							
							onStop(set(stage, value(State::Orbiting)))
						)
					)),
					uninterruptible(sequence(
						predicate(equals(stage, value(State::Orbiting))),
						parallel(
							sequence(
								moveTo(
									orbit,
									Relative(options.Get("OrbitSpeedMultiplier", 1.2f)),
									true
								),
								onStop([=](StateRef state) {
									// is there a better way to do this?
									if (stage(state) == State::Orbiting) {
										stage(state, State::Retreating);
									}
								})
							),
							dropFor(4.s + .5s * counter, set(stage, value(State::Attacking)))
						)
					)),
					uninterruptible(sequence(
						startPredicate(equals(stage, value(State::Retreating))),
						sequence(
							moveTo(
								move::From(
									locator::RandomReachablePointAround(
										locator::Forward(-options.Get("RetreatDistance", 500.f)),
										options.Get("RetreatDistance", 500.f) * .4f
									)
								),
								Relative(options.Get("RetreatSpeedMultiplier", 1.5f))
							),
							onStop(set(stage, value(State::Attacking)))
						)
					))
				))
			)
		),
		every(.5s, parallel(
			sequence(
				set(
					actor::Target(),
					firstValue(
						mostAggresive,
						actor::ClosestPlayer()
					)
				),
				[=](StateRef state) {
					if (state.params().target.HasActor()) {
						const auto target = static_cast<AActor*>(state.params().target);

						const auto targetLocation = target->GetActorLocation();
						const auto ownerForward = state.owner->GetActorForwardVector();

						const auto delta = (targetLocation - state.owner->GetActorLocation()).GetSafeNormal();
						
						sign(state, -FMath::Sign(FVector::CrossProduct(ownerForward, delta).Z));
					}
				}
			),
			sequence(
				predicate(locator::IsInRange(actor::Target(), 2000.f))
			)
		))
	);
}

}}}
