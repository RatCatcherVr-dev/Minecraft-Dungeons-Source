#include "Dungeons.h"
#include "VineBehavior.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/RangedAttackBehavior.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/condition/BtInvert.h"
#include "game/ai/provider/Actors.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/util/Tags.h"
#include "game/ai/action/TransformActions.h"
#include "game/ai/action/CommonActions.h"
#include "game/ai/action/HealthActions.h"

#include "game/ai/task/PlayAnimation.h"
#include "game/ai/provider/ActorStats.h"
#include "game/ai/bt/decorator/BtDropFor.h"
#include "game/ai/debug/BtDebug.h"
#include "game/ai/action/StateActions.h"

#include "game/actor/character/player/PlayerCharacter.h"

namespace bt {
	namespace behavior {
		namespace entities {

			BehaviorTuple createVineSimple(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {

				enum class State {
					Active,
					Activating,
					Deactivating,
				};

				const auto state = sharedRef(State::Activating);

				float activationDistance = options.Get("ActivationDistance", 1000.0f);
				float deactivationDistance = options.Get("DeactivationDistance", 1200.0f);

				return BehaviorTuple(
					selector("VineSimple",
						sequence(predicate(equals(state, value(State::Activating))),
							bt::state::SetIsTargetable(false),
							parallel(
								playAnimation(options.Get("Burst")),
								dropFor(options.Get("WhispererBurstDuration", 0.5s), set(state, value(State::Active)))
							)
						),
						sequence(
							predicate(
								equals(state, value(State::Deactivating)) &&
								actor::IsAlive(actor::Self())
							),
							bt::state::SetCollisionEnabled(false),
							bt::state::SetIsTargetable(false),
							bt::state::SetHealthBarEnabled(false),
							parallel(
								chain(
									playAnimation(options.Get("Drop")),
									playAnimation(options.Get("Dead"), true)
								),
								dropFor(options.Get("WhispererDeathDuration", 0.5s),
									common::Apply(actor::Self(), [](AActor* mob) { mob->Destroy(); })
								)
							)
						),
						sequence(predicate(equals(state, value(State::Active))),
							parallel(
								bt::state::SetIsTargetable(true),
								bt::state::SetHealthBarEnabled(true),
								ifElse(isInRange(actor::Target(), activationDistance, deactivationDistance),
									alwaysTrue(behavior::rangedAttack(mob, 0.0f, activationDistance, deactivationDistance, actor::Target())),
									playAnimation(options.Get("Idle"), true)
								),
								dropFor(options.Get("WhispererDuration", 5.0s), set(state, value(State::Deactivating))),
								sequence(predicate(lessThan(actorStats::healthFraction(), value(options.Get("WhispererDeathHealth", 0.2f)))),
									set(state, value(State::Deactivating))
								)
							)
						)
					),
					parallel(
						every(0.5s,
							set(actor::Target(), actor::ClosestEnemy())
						)
					)
				);
			}

			BehaviorTuple createVine(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {

				const auto canBurst = makeSharedRef<bool>(true);

				float activationDistance = options.Get("ActivationDistance", 1000.0f);
				float deactivationDistance = options.Get("DeactivationDistance", 1200.0f);
				float wakeupHealth = options.Get("WakeupHealth", 0.9f);
				float sleepHealth = options.Get("SleepHealth", 0.3f);
				float healRate = options.Get("HealRate", 0.025f);

				return BehaviorTuple(
					root(selector("Vine",
						uninterruptible(sequence(
							startPredicate(locator::IsInRange(actor::Target(), deactivationDistance)),
							chain(
								sequence(
									predicate(equals(value(canBurst), value(true))),
									playAnimation(options.Get("Burst")),
									set(canBurst, value(false))
								),
								sequence(
									bt::state::SetIsTargetable(true),
									bt::state::SetCollisionEnabled(true),
									bt::state::SetHealthBarEnabled(true),
									locator::IsInRange(actor::Target(), deactivationDistance)
								)
							)
						)),
						sequence(
							predicate(!locator::IsInRange(actor::Target(), deactivationDistance)),
							onStart(bt::state::SetIsTargetable(false)),
							onStart(bt::state::SetCollisionEnabled(false)),
							onStart(bt::state::SetHealthBarEnabled(false)),
							onStart(set(canBurst, value(true))),
							chain(
								sequence(
									playAnimation(options.Get("Drop"))
								),
								parallel(
									set(canBurst, value(true)),
									alwaysTrue(playAnimation(options.Get("Dead"), true))
								)
							)
						)
					)),
					parallel("update-targets",
						every(0.5s,
							set(actor::Target(), actor::ClosestEnemy())
						)
					)
				);
			}
		}
	}
}
