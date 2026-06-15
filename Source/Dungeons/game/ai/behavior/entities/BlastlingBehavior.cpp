#include "Dungeons.h"
#include "BlastlingBehavior.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/RangedAttackComponent.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/FleeBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/RangedAttackBehavior.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/provider/Locators.h"
#include "game/ai/task/move/TeleportTask.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/condition/ActorPredicates.h"
#include "util/FloatRange.h"
#include "game/component/BehaviorOptionsComponent.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/condition/BtPredicate.h"
#include "../../task/PlayAnimation.h"

namespace bt {
	namespace behavior {
		namespace entities {

			BehaviorTuple createBlastling(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
				URangedAttackComponent& rangedComponent = *mob.FindComponentByClass<URangedAttackComponent>();

				static Random rnd;

				const bt::Provider<bt::Duration> teleportDurationProvider = [
					dur = options.Get("DisappearDuration", 1.s),
						longDur = options.Get("DisappearDurationLong", 6.s)
				](StateRef state) {
						return rnd.nextFloat() < .9f ? dur : longDur;
					};								

					const auto setRandRangeAttackVariant = [&rangedComponent]() {
						return [&rangedComponent](StateRef state) {
							rangedComponent.SetRangeAttackVariant(rnd.nextInt(-1, rangedComponent.GetNumAttackVariants()));
						};
					};

					const auto HasStatus = [&](AActor* target) {
						if (ABaseCharacter* charater = Cast<ABaseCharacter>(target))
						{
							if (UAbilitySystemComponent* component = charater->GetAbilitySystemComponent())
							{
								return component->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.VoidTouched")) ||
									component->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Voided"));
							}
						}
						return false;
					};

					const auto RangedMinDistanceBeforeFleeing = options.Get("RangedMinDistanceBeforeFleeing", 700.f);
					const auto RepositionDistance = options.Get("RepositionDistance", 1000.f);
					const auto RepositionAfterNumAttacks = options.Get("RepositionAfterNumAttacks", 1);
					const auto TeleportCooldown = options.Get("TeleportCooldown", 3.s);
					const auto TeleportDuration = options.Get("TeleportDuration", .333s);
					const auto TeleportAwayTriggerRange = options.Get("TeleportAwayTriggerRange", 200.f);
					const auto TeleportAwayMin = options.Get("TeleportAwayMin", 800.f);
					const auto TeleportAwayMax = options.Get("TeleportAwayMax", 850.f);
					const auto MinAttackDistance = options.Get("MinAttackDistance", 10.f);
					const auto RangedAcquireTargetRangeUnits = options.Get("RangedAcquireTargetRangeUnits", 1000.f);
					const auto RangedLoseTargetRangeUnits = options.Get("RangedLoseTargetRangeUnits", 1200.f);
					const auto ChaseSpeedMultiplier = options.Get("ChaseSpeedMultiplier", .5f);
					const auto ChaseIfAttackedSpeedMultiplier = options.Get("ChaseIfAttackedSpeedMultiplier", .6f);
					const auto ChaseIfWarnedSpeedMultiplier = options.Get("ChaseIfWarnedSpeedMultiplier", .6f);

					return BehaviorTuple(
						root(selector("Blastling",
							sequence("flee-state",
								startCondition(isInRange(actor::Target(), RangedMinDistanceBeforeFleeing)),
								behavior::defaultFleeFrom(locator::Target())
							),
							sequence("reposition",
								behavior::defaultFleeFromAfterNumAttacks(mob.MobParams, locator::RandomReachablePointAround(actor::Self(), RepositionDistance), value(RepositionAfterNumAttacks))
							),
							//aggressive teleport
							uninterruptible(sequence(
								startPredicate(
									IsVoided(actor::Target()) &&
									!actor::HasRecentlyTeleported(TeleportCooldown)
								),
								make_unique<UTeleportTask>(
									TeleportDuration,
									TeleportDuration,
									value(0.s),
									locator::RandomReachablePointAround(actor::Target(), /*20.0f, 25.5f),*/ ("MinAttackDistance", 10.f), rangedComponent.GetAttackRange()),
									
									actor::Target()
									)
							)),
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

							uninterruptible(sequence(
								rangedAttack(
								mob,
								MinAttackDistance,
								RangedAcquireTargetRangeUnits,
								RangedLoseTargetRangeUnits,
								actor::Target()
								),
								setRandRangeAttackVariant()
							)),
							behavior::chase(actor::Target(), FloatRange(rangedComponent.GetAttackRange(), RangedAcquireTargetRangeUnits), Relative(ChaseSpeedMultiplier)),
							behavior::chaseIfAttacked(mob, Relative(ChaseIfAttackedSpeedMultiplier)),
							behavior::chaseIfWarned(mob, Relative(ChaseIfWarnedSpeedMultiplier)),
							behavior::defaultRoam()
						)),
						parallel("update-targets",
							every(.5s, set(actor::Target(), actor::ClosestEnemyCanIgnoreFilter(3000.0f, true, HasStatus))
							),
							every(3s, sequence(
								isInRange(actor::Target(), mob.OffensiveRange),
								set(location::Anchor(), location::Self()))
							))
					);
			}

		}
	}
}
