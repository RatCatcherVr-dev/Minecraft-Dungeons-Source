#include "Dungeons.h"
#include "GhastBehavior.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/component/RangedAttackComponent.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/FleeBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/RangedAttackBehavior.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/provider/Locators.h"
#include "game/ai/task/PlayAnimation.h"
#include "game/ai/bt/decorator/BtDropFor.h"
#include "util/FloatRange.h"
#include "game/component/BehaviorOptionsComponent.h"
#include "game/ai/task/attack/RangedAttack.h"

namespace bt { namespace behavior { namespace entities {

BehaviorTuple createGhast(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
	enum class State {
		Far,
		Tween,
		Near
	};

	const auto stage = sharedRef(State::Far);

	const auto offsetZ = options.Get("OffsetZ", 1000.f);
	const auto forwardMultiplier = options.Get("ForwardMultiplier", 0.f);
	const auto offsetRadiusMax = options.Get("OffsetRadiusMax", 0.f);

	const auto canHit = [=](UWorld& world, FVector source, const AActor* targetActor) {
		const auto radius = FMath::FRandRange(0.f, offsetRadiusMax);
		const auto angle = FMath::FRandRange(0.f, PI * 2.f);

		const auto targetLocation = targetActor->GetActorLocation() +
			forwardMultiplier * targetActor->GetVelocity() +
			FVector { FMath::Cos(angle) * offsetRadiusMax, FMath::Sin(angle) * offsetRadiusMax, 0.f };

		FHitResult hitResult;
		world.LineTraceSingleByChannel(hitResult, source, targetLocation, static_cast<ECollisionChannel>(ECustomTraceChannels::TerrainAndPlayerOnly));
		return hitResult.Actor == targetActor;
	};

	return BehaviorTuple(
		selector("Ghast",
			sequence(
				predicate(equals(stage, value(State::Far))),
				sequence(
					predicate([=](StateRef state) {
						const AActor* target = state.params().target;
						if (target == nullptr) {
							return false;
						}

						return canHit(
							state.world(),
							state.owner->GetActorLocation() + FVector { 0.f, 0.f, offsetZ },
							target
						);
					}),
					alwaysTrue(make_unique<URangedAttack>(
						actor::Target(),
						[=](StateRef state) { return state.owner->GetActorLocation() + FVector { 0.f, 0.f, offsetZ }; }
					)),
					alwaysTrue(playAnimation(options.Get("Hover"), true, false)), // alwaysTrue?
					alwaysTrue(sequence(
						predicate(actor::IsAnyPlayersInRange(options.Get("WakeupRange", 1500.f))),
						set(stage, value(State::Tween))
					)),
					onStop([](StateRef state) {
						const auto rangedAttackComponent = state.owner->FindComponentByClass<URangedAttackComponent>();
						rangedAttackComponent->ResetOrigin();
					})
				)
			),
			sequence(
				predicate(equals(stage, value(State::Tween))),
				alwaysTrue(playAnimation(options.Get("Descend"))), // alwaysTrue?
				alwaysTrue(dropFor(options.Get("DescendDuration", 2.7s),
					set(stage, value(State::Near))
				))				
			),
			sequence(
				predicate(equals(stage, value(State::Near))),
				selector(
					sequence(
						startCondition(isInRange(actor::Target(), options.Get("FleeTriggerRange", 400.f))),
						behavior::defaultFleeFromAfterNumAttacks(mob.MobParams, locator::Target(), value(options.Get("FleeTriggerCount", 2)))
					),
					rangedAttack(
						mob,
						options.Get("NearMinAttackDistance", 10.f),
						options.Get("NearRangedAcquireTargetRangeUnits", 2000.f),
						options.Get("NearRangedLoseTargetRangeUnits", 2500.f),
						actor::Target()
					),
					behavior::defaultRoam()
				)
			)
		),
		parallel("update-targets",
			every(.5s, set(actor::Target(), actor::ClosestEnemy(5000.f))),
			every(3s, sequence(
				isInRange(actor::Target(), mob.OffensiveRange),
				set(location::Anchor(), location::Self())
			))
		)
	);
}

}}}
