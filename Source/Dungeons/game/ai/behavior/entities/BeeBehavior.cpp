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
#include "game/ai/action/HealthActions.h"
#include "game/actor/BeeNest.h"
#include <GameplayTagContainer.h>
#include "util/Algo.h"
#include "game/ai/bt/BtDelay.h"
#include "game/mobspawn/MobAction.h"

namespace bt { namespace behavior { namespace entities {

using Provider = bt::Provider<AActor*>;

Provider closestBeePreferedPlayer(float maxDistance = 2000.f) {
	return [maxDistance](StateRef state) -> AActor* {
		const float squared = maxDistance * maxDistance;
		const float likedScore = squared * 4;
		const float masterScore = squared * 2;

		//Bad bees do not want to hang about anyone, they are bad bad murder bees
		if (state.owner->GetCurrentTeam() == ETeamName::Villains) {
			return nullptr;
		} else {
			//Nice bees look for players to hang about
			return algo::max_element_by(actorquery::getNearbyInstanceTrackedActors<APlayerCharacter>(state.owner, maxDistance), [&](const APlayerCharacter* v) {
				auto abilitySystem = v->GetAbilitySystemComponent();
				//We like bee nest wearing players the most
				float isLikeByBees = abilitySystem->HasAnyMatchingGameplayTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.BeeMine")))) ? likedScore : 0.f;
				//And our master the second most
				float isMaster = state.owner->GetMaster() == Cast<const ABaseCharacter>(v) ? masterScore : 0;
				//And distance as a tie breaker
				return squared - actorquery::getActorDistanceSquared(state.owner, v) + isLikeByBees + isMaster;
			}).Get(nullptr);
		}
	};
}

Provider closestBeeNest(float maxDistance = 2000.f) {
	return [maxDistance](StateRef state) -> AActor* {
		return algo::min_element_by(actorquery::getNearbyInstanceTrackedActors<ABeeNest>(state.owner, maxDistance), [&](const ABeeNest* v) {
			return actorquery::getActorDistanceSquared(state.owner, v);
		}).Get(nullptr);
	};
}

BehaviorTuple createBee(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
	static Random rnd;

	const auto rightHand = rnd.nextBoolean() ? -1.f : 1.f;
	const auto nestRange = options.Get("NestRange", 2000.f);
	const auto homeRange = options.Get("FindHomeRange", 2000.f);

	const auto petTarget = actorquery::is::tagged(tags::petTarget);

	const auto isPetEnemy = [&](const AActor* actor) -> bool {
		const auto target = Cast<AMobCharacter>(actor);
		return target != nullptr &&  mob.IsHostileTowards(target) && target->IsTargetable();
	};

	auto Home = SharedObjectRef<AActor>();

	return BehaviorTuple(
		root(selector("Bee",
			uninterruptible(sequence(
				startPredicate(locator::IsInRange(Home, { options.Get("AttackTriggerDistance", 1000.f) })),
				selector(
					sequence("suicide",
						predicate(greaterThanOrEquals(ref(mob.MobParams.successfulAttacks.all), value(options.Get("AttacksUntilDeath", 3)))),
						chain(
						delay(options.Get("SuicideDelay", 0.2s)),
							health::Kill(actor::Self(), damageTag::melee())
						)
					),
					sequence("attack",
						isInRange(
							actor::Target(),
							options.Get("AttackEnterDistance", 350.f),
							options.Get("AttackExitDistance", 500.f)
						),
						ifElse(
							isInRange(
								actor::Target(),
								options.Get("AttackChargeRange", 300.f)
							),
							behavior::meleeAttack(mob),
							behavior::moveTo(actor::Target(), Relative(options.Get("AttackChargeMultiplier", .9f)))
						)
					),
					behavior::chaseInOffensiveRange(mob, actor::Target(), Relative(options.Get("ChaseMultiplier", .6f)))
				)
			)),
			uninterruptible(sequence(
				startPredicate(
					!locator::IsInRange(Home, { options.Get("FollowTriggerDistance", 1300.f) }) ||
					(
						!locator::IsInRange(Home, { options.Get("AttackTriggerDistance", 1000.f) }) &&
						lessThan(
							actor::CountInRange(Home, 1000.f, isPetEnemy),
							value(5)
						)
					)
				),
				follow( Home,
					[
						rightHand,
						aheadDistance = options.Get("AheadDistance", 400.f),
						sideDistance = options.Get("SideDistance", 200.f),
						Home
					](StateRef state) {
						if (const auto master = Home) {
							if (locationquery::isInFrontOf(master, state.owner->GetActorLocation())) {
								return master->GetActorLocation() + master->GetActorForwardVector() * aheadDistance;
							}
							else {
								return master->GetActorLocation() + master->GetActorForwardVector().RotateAngleAxis(45.f * rightHand, FVector::UpVector) * sideDistance;
							}
						}
						else {
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
			behavior::smoothRoam(options.Get("RoamInterval", 3.s))
		)),
		every(
			1s * rnd.nextFloat(.25f, .5f),
			sequence(
			set(
				Home,
				firstValue(
					closestBeeNest(homeRange),
					closestBeePreferedPlayer(homeRange)
				)
			),
			set(
				actor::Target(),
				firstValue(
					actor::ClosestMob(2000.f, actorquery::is::tagged(tags::petTarget)),
					actor::ClosestMob(1500.f, isPetEnemy)
				)
			))
		)
	);
}

}}}
