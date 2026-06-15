#include "Dungeons.h"
#include "ZombieBehavior.h"
#include "game/ai/action/CommonActions.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/MeleeAttackBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/provider/Locators.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/condition/BtProbability.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/condition/AttackPredicates.h"
#include "game/component/BehaviorOptionsComponent.h"
#include "game/component/MeleeAttackComponent.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "util/Random.h"

class UBehaviorOptionsComponent;

namespace bt { namespace behavior { namespace entities {

FVector getLocationAroundOwner(const FVector& ownLocation, const FVector& targetLocation, float angleOffset) {
	const auto halfDelta = (targetLocation - ownLocation).GetUnsafeNormal() * 400.f;
	return ownLocation + halfDelta.RotateAngleAxis(angleOffset, FVector::UpVector);
}

FVector getLocationAroundTarget(const FVector& ownLocation, const FVector& targetLocation, float angleOffset) {
	const auto halfDelta = (ownLocation - targetLocation).GetUnsafeNormal() * 300.f;
	return targetLocation + halfDelta.RotateAngleAxis(angleOffset, FVector::UpVector);
}

BehaviorTuple createZombie(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
	static Random rnd;

	const auto angleSign = rnd.nextFloat() < .5f ? 1 : -1;

	enum class State {
		Chasing,
		Flanking,
		Idling
	};

	const auto stage = std::make_shared<State>(State::Idling);	
	
	auto flankLocation = std::make_shared<FAIMoveRequest>();

	const auto speedMultiplier = rnd.nextFloat(options.Get("WalkSpeedFactorMin", 0.4f), options.Get("WalkSpeedFactorMax", 0.6f));

	const auto AttackInDistance = options.Get("AttackInDistance", 250.f);
	const auto AttackOutDistance = options.Get("AttackOutDistance", 400.f);

	return BehaviorTuple(
		selector("Zombie",
			sequence("Attack",
				isInRange(
					actor::Target(),
					AttackInDistance,
					AttackOutDistance
				),
				ifElse(attack::InAttackRangeOrAttacking(actor::Target(), mob.FindComponentByClass<UMeleeAttackComponent>()),
					alwaysTrue(behavior::meleeAttack(mob)),
					behavior::moveTo(actor::Target(), Relative(speedMultiplier))
				)
			),
			sequence(
				predicate(!locator::IsInRange(actor::Target(), AttackInDistance)),
				selector(
					behavior::chaseIfAttacked(mob, Relative(speedMultiplier * 1.2f)),
					sequence("chasing",
						predicate([stage](StateRef) { return *stage == State::Chasing; }),
						behavior::chaseInOffensiveRange(mob, actor::Target(), Relative(speedMultiplier))
					),
					sequence("flanking",
						predicate([stage](StateRef) { return *stage == State::Flanking; }),
						behavior::moveTo([flankLocation](auto state) { return *flankLocation; }, Relative(speedMultiplier * 1.2f))
					),
					behavior::chaseIfWarned(mob, Relative(speedMultiplier * 1.2f)),
					behavior::defaultRoam()
				)
			)			
		),
		parallel("update-targets",
			every(.8s * rnd.nextFloat(.7f, 1.3f),
				sequence(
					set(actor::Target(), actor::ClosestEnemy()),
					selector(
						sequence(
							predicate(!locator::IsInRange(locator::Target(), FloatRange { 3000.f })),
							common::Exec([stage] (StateRef state) { 
								*stage = State::Idling;
								state.owner->ResetLookAtActor();
							})
						),
						sequence(
							predicate(!locator::IsInRange(locator::Target(), FloatRange { 1000.f })),
							predicate(random::probability(.8f)),
							common::Exec([stage, flankLocation, angleSign](StateRef state) {
								if (const auto target = actor::Target()(state)) {
									*stage = State::Flanking;
									state.owner->SetLookAtActor(target);
									const auto location = getLocationAroundOwner(state.owner->GetActorLocation(), target->GetActorLocation(), rnd.nextFloat(20.f, 40.f) * angleSign);
									//DrawDebugSphere(&state.world(), location, 10, 8, FColor::Yellow, true, 2.f);
									*flankLocation = move::Location(value(location))(state);
								}
							})
							//common::Exec([] { UE_LOG(LogTemp, Warning, TEXT("spreading")); }) // make an action for logging like this
						),
						sequence(
							predicate(!locator::IsInRange(locator::Target(), FloatRange { 500.f })),
							predicate(random::probability(.6f)),
							common::Exec([stage, flankLocation, angleSign] (StateRef state) {
								if (const auto target = actor::Target()(state)) {									
									*stage = State::Flanking;
									state.owner->SetLookAtActor(target);
									const auto location = getLocationAroundTarget(state.owner->GetActorLocation(), target->GetActorLocation(), rnd.nextFloat(20.f, 80.f) * -angleSign);
									//DrawDebugSphere(&state.world(), location, 10, 8, FColor::White, true, 2.f);
									*flankLocation = move::Location(value(location))(state);
								}
							})
							//common::Exec([] { UE_LOG(LogTemp, Warning, TEXT("flanking")); })
						),
						sequence(
							common::Exec([stage](StateRef state) {
								if (const auto target = actor::Target()(state)) {									
									*stage = State::Chasing;
									state.owner->SetLookAtActor(target);
								}
							})
							//common::Exec([] { UE_LOG(LogTemp, Warning, TEXT("chasing")); })
						)
					)
				)
			),
			every(3s, sequence(
				isInRange(actor::Target(), mob.OffensiveRange),
				set(location::Anchor(), location::Self())
			))
		)
	);
}

}}}
