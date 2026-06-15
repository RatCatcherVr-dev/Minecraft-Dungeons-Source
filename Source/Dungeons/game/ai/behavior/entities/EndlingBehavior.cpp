#include "Dungeons.h"
#include "EndlingBehavior.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/MeleeAttackBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/provider/Actors.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/bt/decorator/BtDropFor.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/task/move/TeleportTask.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/provider/Locators.h"
#include "game/util/LocationQuery.h"
#include "game/ai/task/PlayAnimation.h"
#include "game/ai/action/StateActions.h"
#include "util/Random.h"
#include <AIController.h>
#include "game/component/MeleeAttackComponent.h"
#include "game/ai/condition/AttackPredicates.h"

namespace bt { namespace behavior { namespace entities {

	BehaviorTuple createEndling(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
		static Random rnd;

		enum class State {
			Idle,
			Perturbated,
			PreAggressive,
			Aggressive
		};

		const auto stage = sharedRef(State::Idle);

		const auto AggresiveCloseTriggerRange = options.Get("AggresiveCloseTriggerRange", 700.f);
		const auto TeleportCooldown2s = options.Get("TeleportCooldown", 2.s);
		const auto TeleportCooldownIgnoreDamaged = options.Get("TeleportCooldownIgnoreDamaged", 3.5s);
		const auto TeleportBehindTriggerRange = options.Get("TeleportBehindTriggerRange", 200.f);
		const auto TeleportOutDuration = options.Get("TeleportOutDuration", .667s);
		const auto TeleportInDuration = options.Get("TeleportInDuration", .667s);
		const auto TeleportBehindOffset = options.Get("TeleportBehindOffset", -500.f);
		const auto TeleportCooldown3s = options.Get("TeleportCooldown", 3.s);
		const auto TeleportInFrontRecentlyAttackedThreshold = options.Get("TeleportInFrontRecentlyAttackedThreshold", 3.s);
		const auto TeleportInFrontTriggerRange = options.Get("TeleportInFrontTriggerRange", 400.f);
		const auto TeleportInFrontOutDuration = options.Get("TeleportInFrontOutDuration", .333s);
		const auto TeleportInFrontInDuration = options.Get("TeleportInFrontInDuration", .333s);
		const auto TeleportInFrontOffset = options.Get("TeleportInFrontOffset", 350.f);
		const auto AttackInDistance = options.Get("AttackInDistance", 250.f);
		const auto AttackOutDistance = options.Get("AttackOutDistance", 400.f);
		const auto AttackChaseMultiplier = options.Get("AttackChaseMultiplier", 1.3f);
		const auto ChaseMultiplier = options.Get("ChaseMultiplier", 1.5f);

		const bt::Provider<bt::Duration> teleportDurationProvider = [
			dur = options.Get("DisappearDuration", 1.s),
				longDur = options.Get("DisappearDurationLong", 6.s)
		](StateRef state) {
				return rnd.nextFloat() < .9f ? dur : longDur;
			};

		return BehaviorTuple(


			selector("Endling",
				sequence("idle",
					predicate(equals(stage, value(State::Idle))),
					parallel(
						sequence(
							isInRange(actor::Target(), 1500.f),
							set(stage, value(State::Perturbated))
						),
						behavior::defaultRoam(),
						sequence(
							predicate(actor::IsRecentlyDamagedFromAttack(.5s)),

							set(stage, value(State::PreAggressive)
							)
						)
					)
				),
				sequence("perturbated",
					predicate(equals(stage, value(State::Perturbated))),
					parallel(
						//playAnimation(options.Get("Perturbating"), true),
						sequence(
							predicate(!locator::IsInRange(actor::Target(), 1500.f)),
							set(stage, value(State::Idle))
						),

						sequence(
							predicate(
								actor::IsRecentlyDamagedFromAttack(.5s) ||
								locator::IsInRange(actor::Target(), AggresiveCloseTriggerRange)
							),
							set(stage, value(State::PreAggressive))
						)
					)
				),
				sequence("pre-aggressive",
					predicate(equals(stage, value(State::PreAggressive))),
					chain(
						//playAnimation(options.Get("Unleash")),
						set(stage, value(State::Aggressive))
					)
				),
				sequence("aggressive",
					predicate(equals(stage, value(State::Aggressive))),
					root(selector(
						sequence(
							predicate(!locator::IsInRange(actor::Target(), 2000.f)),
							set(stage, value(State::Idle))
						),
						uninterruptible(sequence(
							startPredicate(
								!actor::HasRecentlyTeleported(TeleportCooldown2s) &&
								(!actor::IsRecentlyDamagedFromAttack(.300s) || !actor::HasRecentlyTeleported(TeleportCooldownIgnoreDamaged)) && // let's not disappear immediately after hit
								locator::IsInRange(actor::Target(), TeleportBehindTriggerRange) &&
								actor::IsBeingLookedAt(actor::Target(), PI * .2f)
							),
							make_unique<UTeleportTask>(
								TeleportOutDuration,
								TeleportInDuration,
								teleportDurationProvider,
								locator::Forward(actor::Target(), TeleportBehindOffset),
								actor::Target()
								)
						)),
						uninterruptible(sequence(
							startPredicate(
								!actor::HasRecentlyTeleported(TeleportCooldown3s) &&
								!actor::HasRecentlyAttacked(TeleportInFrontRecentlyAttackedThreshold) &&
								!locator::IsInRange(actor::Target(), TeleportInFrontTriggerRange) &&
								!actor::IsBeingLookedAt(actor::Target(), PI * .2f)
							),
							make_unique<UTeleportTask>(
								TeleportInFrontOutDuration,
								TeleportInFrontInDuration,
								teleportDurationProvider,
								locator::Forward(actor::Target(), TeleportInFrontOffset),
								actor::Target()
								)
						)),
						sequence("Attack",
							isInRange(
								actor::Target(),
								AttackInDistance,
								AttackOutDistance
							),
							ifElse(attack::InAttackRangeOrAttacking(actor::Target(), mob.FindComponentByClass<UMeleeAttackComponent>()),
								alwaysTrue(behavior::meleeAttack(mob)),
								behavior::moveTo(actor::Target(), Relative(AttackChaseMultiplier))
							)
						),
						behavior::chaseInOffensiveRange(mob, actor::Target(), Relative(ChaseMultiplier)),
						behavior::defaultRoam()
					))
				)
			),
			parallel("update-targets",
				every(0.5s, set(actor::Target(), actor::ClosestEnemy(5000.f))),
				every(3s, sequence(
					isInRange(actor::Target(), mob.OffensiveRange),
					set(location::Anchor(), location::Self())
				))
			)
		);
	}

}}}
