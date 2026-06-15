#include "Dungeons.h"
#include "PoisonVineBehavior.h"
#include "game/ai/bt/BtDelay.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/RangedAttackBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/provider/Actors.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/util/Tags.h"
#include "game/ai/action/TransformActions.h"
#include "game/ai/action/CommonActions.h"
#include "game/ai/action/HealthActions.h"
#include "game/ai/action/FocusActions.h"

#include "game/ai/task/PlayAnimation.h"
#include "game/ai/task/attack/RangedAttack.h"
#include "game/ai/provider/ActorStats.h"
#include "game/ai/bt/decorator/BtDropFor.h"
#include "game/ai/debug/BtDebug.h"
#include "game/ai/action/StateActions.h"

#include "game/actor/character/player/PlayerCharacter.h"

namespace bt { namespace behavior { namespace entities {

	BehaviorTuple createPoisonVineInternal(AMobCharacter& mob, const UBehaviorOptionsComponent& options, bool whisperer) {

		enum class State {
			Active,
			Activating,
			Attacking,
			Deactivating,
		};

		const auto state = sharedRef( State::Activating );

		float activationDistance = options.Get("ActivationDistance", 1000.0f);
		float deactivationDistance = options.Get("DeactivationDistance", 1200.0f);
		auto attackDelay = options.Get("AttackDelay", 5.0s);
		bt::Duration duration = 999999.0s;
		
		if (whisperer) {
			duration = options.Get("WhispererDuration", 5.0s);
			attackDelay = options.Get("WhispererAttackDelay", 5.0s);
		}

		auto closestFilter = [&](AActor* actor) {
			const float zlimit = 300.0f;
			return FMath::Abs( actor->GetActorLocation().Z - mob.GetActorLocation().Z ) < zlimit;
		};

		return BehaviorTuple(
			root(selector(
				uninterruptible(sequence("activating",
					predicate(equals(state, value(State::Activating))),
					onStop(set(state, value(State::Attacking))),
					bt::state::SetIsTargetable(false),
					//focus::Set(actor::Target(), EAIFocusPriority::Gameplay),
					playAnimation(options.Get("Burst"), false, false)
				)),
				sequence("attacking",
					predicate(equals(state, value(State::Attacking))),
					selector(
						sequence(
							predicate(!locator::IsInRange(actor::Target(), activationDistance)),
							set(state, value(State::Active))
						),
						sequence(
							chain(
								make_unique<URangedAttack>(actor::Target()),
								set(state, value(State::Active))
							)
						)
					)
				),
				sequence("active",
					predicate(equals(state, value(State::Active))),
					bt::state::SetIsTargetable(true),
					parallel(
						//alwaysTrue(make_unique<UTurnTask>(actor::Target(), 300.f)),
						dropFor(attackDelay, set(state, value(State::Attacking))),
						alwaysTrue(ifElse(isInRange(actor::Target(), activationDistance),
							playAnimation(options.Get("Armed"), true),
							playAnimation(options.Get("Idle"), true)
						))
					)
				),
				sequence("deactivating",
					predicate(equals(state, value(State::Deactivating))),
					parallel(
						playAnimation(options.Get("Dead"), true),
						dropFor(options.Get("WhispererDeathDuration", 0.5s),
							common::Apply(actor::Self(), [](AActor* mob) { mob->Destroy(); })
						)
					)
				)
			)),
			parallel("update-targets",
				every(0.5s, sequence(
					set( actor::Target(), actor::ClosestPlayer( 20000.0f, closestFilter )),
					focus::Set(actor::Target(), EAIFocusPriority::Attacking)
				)),
				alwaysTrue(dropFor(duration, set( state, value( State::Deactivating ) ) ))
			)
		);
	}

	BehaviorTuple createPoisonVineSimple(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
		return createPoisonVineInternal(mob, options, true);
	}

	BehaviorTuple createPoisonVine(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
		return createPoisonVineInternal(mob, options, false);
	}
}}}
