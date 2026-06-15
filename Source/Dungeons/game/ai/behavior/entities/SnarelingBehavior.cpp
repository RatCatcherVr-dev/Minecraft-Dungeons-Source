#include "Dungeons.h"
#include "SnarelingBehavior.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/component/MeleeAttackComponent.h"
#include "game/ai/behavior/RangedAttackBehavior.h"
#include "game/ai/behavior/MeleeAttackBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/behavior/FleeBehavior.h"
#include "game/ai/behavior/ChaseAndMeleeAttackBehavior.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/provider/Actors.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/util/Tags.h"
#include "game/ai/action/TransformActions.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/task/move/TeleportTask.h"
#include "game/actor/character/player/PlayerCharacter.h"


namespace bt { namespace behavior { namespace entities {
	BehaviorTuple createSnareling(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {

		static Random rnd;

		const bt::Provider<bt::Duration> teleportDurationProvider = [
			dur = options.Get("DisappearDuration", 1.s),
				longDur = options.Get("DisappearDurationLong", 6.s)
		](StateRef state) {
				return rnd.nextFloat() < .9f ? dur : longDur;
			};

			const auto HasStatus = [&](AActor* target) {
				if (ABaseCharacter* charater = Cast<ABaseCharacter>(target))
				{
					if (UAbilitySystemComponent* component = charater->GetAbilitySystemComponent())
					{
						return component->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("StatusEffect.Immobile.SpiderWeb"));
					}
				}
				return false;
			};

			const auto TeleportCooldown = options.Get("TeleportCooldown", 3.s);
			const auto TeleportAwayTriggerRange = options.Get("TeleportAwayTriggerRange", 500.f);
			const auto TeleportInMax = options.Get("TeleportInMax", 150.f);
			const auto TeleportDuration = options.Get("TeleportDuration", .333s);
			const auto RangedAcquireTargetRangeUnits = options.Get("RangedAcquireTargetRangeUnits", 1000.f);
			const auto RangedLoseTargetRangeUnits = options.Get("RangedLoseTargetRangeUnits", 1200.f);
			const auto TeleportAwayMin = options.Get("TeleportAwayMin", 800.f);
			const auto TeleportAwayMax = options.Get("TeleportAwayMax", 850.f);

			return BehaviorTuple(

				root(selector("Snareling",
					sequence(
						predicate(actor::HasTag(tags::inLove)),
						behavior::chaseAndMeleeAttack(mob, options)
					),

					sequence("melee-attack",
						predicate(actor::IsStuckInWeb(actor::Target())|| actor::IsSlowed(actor::Target())),
						selector(
							//aggressive teleport
							uninterruptible(sequence(
								startPredicate(
									!actor::HasRecentlyTeleported(TeleportCooldown) &&
									!locator::IsInRange(actor::Target(), TeleportAwayTriggerRange)
								),
								make_unique<UTeleportTask>(
									TeleportDuration,
									TeleportDuration,
									value(0.s),
									locator::RandomReachablePointAround(actor::Target(), TeleportInMax),
									actor::Target()
									)
							)),
							sequence(
								behavior::meleeAttack(mob),
								behavior::meleeAttack(mob)),
							behavior::moveTo(actor::Target(), Relative(0.5f))
						)
					),

					behavior::rangedAttack(mob, 650, RangedAcquireTargetRangeUnits, RangedLoseTargetRangeUnits),
					
					//escape teleport
					uninterruptible(sequence(
						startPredicate(
							!actor::HasRecentlyTeleported(TeleportCooldown) &&
							locator::IsInRange(actor::Target(), TeleportAwayTriggerRange)
						),
						make_unique<UTeleportTask>(
							TeleportDuration,
							TeleportDuration,
							teleportDurationProvider,
							locator::RandomReachablePointAround(actor::Target(), TeleportAwayMin, TeleportAwayMax),
							actor::Target()
							)
					)),
					every(1s,
						sequence("flee",
							isInRange(actor::Target(), 700),
							predicate(!actor::IsStuckInWeb(actor::Target())),
							behavior::defaultFleeFrom(locator::Target())
						)
					),
					every(1.21s,
						behavior::chaseInOffensiveRange(mob, actor::Target(), Relative(0.4f))
					),


					behavior::chaseIfAttacked(mob, Relative(0.5f)),
					behavior::chaseIfWarned(mob, Relative(0.5f)),
					behavior::defaultRoam()
				)),
				sequence("update-targets",

					every(.5s, sequence(set(actor::Target(), actor::ClosestPlayer(3000.0f, HasStatus)),
						startPredicate(!IsStuckInWeb(actor::Target())),
						set(actor::Target(), actor::ClosestEnemy()))),

					every(0.5s,
						sequence(
						set(actor::Target(), actor::ClosestEnemy())
						)
					),
					every(3s,
						set(location::Anchor(), location::Self())
					)
				)
			);
	}

}
}
}
