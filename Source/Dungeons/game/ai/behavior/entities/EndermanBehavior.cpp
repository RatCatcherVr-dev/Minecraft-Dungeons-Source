#include "Dungeons.h"
#include "EndermanBehavior.h"
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
#include "game/ai/action/FocusActions.h"
#include "game/ai/action/StateActions.h"
#include "util/Random.h"
#include <AIController.h>
#include "game/component/MeleeAttackComponent.h"
#include "game/ai/condition/AttackPredicates.h"
#include "game/component/AwarenessComponent.h"

namespace bt { namespace behavior { namespace entities {

	BehaviorTuple createEnderman(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
		static Random rnd;

		enum class State {
			Idle,
			Perturbated,
			PreAggressive,
			Aggressive
		};

		const auto stage = sharedRef(State::Idle);		
		
		const bt::Provider<bt::Duration> teleportDurationProvider = [
				dur = options.Get("DisappearDuration", 1.s),
				longDur = options.Get("DisappearDurationLong", 6.s)
			](StateRef state) {
				return rnd.nextFloat() < .9f ? dur : longDur;
			};

		mob.SetEventMobRevealed(false);
		const auto awarenessComponent = mob.FindComponentByClass<UAwarenessComponent>();

		return BehaviorTuple(
			selector("Enderman",
				sequence("idle",
					predicate(equals(stage, value(State::Idle))),
					selector("event-mob", state::SetEventMobRevealed(false)),
					parallel(
						sequence(
							predicate([=](StateRef state) { return awarenessComponent->GetFraction() > 0.f; }),
							set(stage, value(State::Perturbated))
						),
						behavior::defaultRoam(),
						sequence(
							predicate(actor::IsRecentlyDamagedFromAttack(.5s)),
							parallel(
								[=](StateRef state) { awarenessComponent->ForceTrigger(); },
								set(stage, value(State::PreAggressive))
							)
						)
					)
				),
				sequence("perturbated",
					predicate(equals(stage, value(State::Perturbated))),
					selector("event-mob", state::SetEventMobRevealed(true)),
					onStart(focus::Set(actor::Target(), EAIFocusPriority::Gameplay)),
					parallel(
						playAnimation(options.Get("Perturbating"), true),
						selector(
							sequence(
								predicate([=](StateRef state) { return awarenessComponent->IsTriggered(); }),
								parallel(
									[=](StateRef state) { awarenessComponent->ForceTrigger(); },
									set(stage, value(State::PreAggressive))
								)
							),
							sequence(
								predicate([=](StateRef state) { return awarenessComponent->GetFraction() <= 0.f; }),
								parallel(								
									focus::Clear(EAIFocusPriority::Gameplay),
									set(stage, value(State::Idle))
								)
							)
						),						
						sequence(
							predicate(
								actor::IsRecentlyDamagedFromAttack(.5s) ||
								locator::IsInRange(actor::Target(), options.Get("AggresiveCloseTriggerRange", 200.f))
							),
							parallel(
								[=](StateRef state) { awarenessComponent->ForceTrigger(); },
								set(stage, value(State::PreAggressive))
							)
						)
					)
				),
				sequence("pre-aggressive",
					predicate(equals(stage, value(State::PreAggressive))),
					selector("event-mob", state::SetEventMobRevealed(true)),
					chain(
						playAnimation(options.Get("Unleash")),
						set(stage, value(State::Aggressive))
					)
				),
			sequence("aggressive",
				predicate(equals(stage, value(State::Aggressive))),
				selector("event-mob", state::SetEventMobRevealed(true)),
				root(selector(
					uninterruptible(sequence(
						startPredicate(
							!actor::HasRecentlyTeleported(options.Get("TeleportCooldown", 2.s)) &&
							(!actor::IsRecentlyDamagedFromAttack(.300s) || !actor::HasRecentlyTeleported(options.Get("TeleportCooldownIgnoreDamaged", 3.5s))) && // let's not disappear immediately after hit
							locator::IsInRange(actor::Target(), options.Get("TeleportBehindTriggerRange", 200.f)) &&
							actor::IsBeingLookedAt(actor::Target(), PI * .2f)
						),
						make_unique<UTeleportTask>(
							options.Get("TeleportOutDuration", .667s),
							options.Get("TeleportInDuration", .667s),
							teleportDurationProvider,
							locator::Forward(actor::Target(), options.Get("TeleportBehindOffset", -500.f)),
							actor::Target()
						)
					)),
					uninterruptible(sequence(
						startPredicate(
							!actor::HasRecentlyTeleported(options.Get("TeleportCooldown", 3.s)) &&
							!actor::HasRecentlyAttacked(options.Get("TeleportInFrontRecentlyAttackedThreshold", 3.s)) &&
							!locator::IsInRange(actor::Target(), options.Get("TeleportInFrontTriggerRange", 400.f)) &&
							!actor::IsBeingLookedAt(actor::Target(), PI * .2f)
						),
						make_unique<UTeleportTask>(
							options.Get("TeleportInFrontOutDuration", .333s),
							options.Get("TeleportInFrontInDuration", .333s),
							teleportDurationProvider,
							locator::Forward(actor::Target(), options.Get("TeleportInFrontOffset", 350.f)),
							actor::Target()
						)
					)),
					sequence("Attack",
						isInRange(
							actor::Target(),
							options.Get("AttackInDistance", 250.f),
							options.Get("AttackOutDistance", 400.f)
						),
						ifElse(attack::InAttackRangeOrAttacking(actor::Target(), mob.FindComponentByClass<UMeleeAttackComponent>()),
							alwaysTrue(behavior::meleeAttack(mob)),
							behavior::moveTo(actor::Target(), Relative(options.Get("AttackChaseMultiplier", 1.3f)))
						)
					),
					behavior::chaseInOffensiveRange(mob, actor::Target(), Relative(options.Get("ChaseMultiplier", 1.5f))),
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
