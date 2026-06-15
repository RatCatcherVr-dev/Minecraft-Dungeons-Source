#include "Dungeons.h"
#include "HoglinBehavior.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/MeleeAttackBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/provider/Move.h"
#include "game/ai/provider/Locations.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/component/BehaviorOptionsComponent.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/task/PlayAnimation.h"
#include "game/ai/action/FocusActions.h"
#include "game/ai/bt/BtDelay.h"
#include "game/component/MeleeAttackComponent.h"
#include "game/ai/condition/AttackPredicates.h"
#include "game/ai/behavior/AoeAttackBehavior.h"
#include "game/ai/bt/decorator/BtDropFor.h"
#include "game/ai/provider/ActorStats.h"

namespace bt { namespace behavior { namespace entities {

	BehaviorTuple createHoglin(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {

		const auto ChaseSpeed = options.Get("ChaseSpeed", 0.5f);

		const auto ChargeMinDistance = options.Get("ChargeMinDistance", 400.0f);
		const auto ChargeMaxDistance = options.Get("ChargeMaxDistance", 2000.0f);
		const auto ChargeSpeed = options.Get("ChargeSpeed", 1.8f);
		const auto ChargeCooldown = options.Get("ChargeCooldown", 8.0s);
		const auto ChargeAttackDuration = options.Get("ChargeAttackDuration", 2.0s);
		const auto ChargeMaxDuration = options.Get("ChargeMaxDuration", 1.75s);
		const auto ChargeUpDuration = options.Get("ChargeUpDuration", 3.5s);
		
		const auto doCharge = [&]() {
			return uninterruptibleSequence("charge-attack",
				startPredicate(
					locator::IsInRange(actor::Target(), ChargeMaxDistance) &&
					!locator::IsInRange(actor::Target(), ChargeMinDistance) &&
					actor::IsAlive(actor::Target()) &&
					actor::IsInLineOfSight(actor::Target())
				),
				minTimeBetweenStopAndStart(ChargeCooldown),
				onStart(focus::Set(actor::Target(), EAIFocusPriority::Attacking)),
				onStop(focus::Clear(EAIFocusPriority::Attacking)),
				chain("charge-or-hit",
					playAnimation(options.Get("ChargeStart"), false, true, true),
					delay(ChargeUpDuration),
					sequence("charge",
						playAnimation(options.Get("Charge"), true),
						behavior::chargeTo(actor::Target(), Relative(ChargeSpeed), ChargeMaxDuration, 5.0s, Relative(ChargeSpeed), 150.0f)
					),
					sequence("charge-melee",
						behavior::meleeAttack(mob, actor::Target(), 1),
						delay(ChargeAttackDuration)
					)								
				)

			);
		};

		return BehaviorTuple(
			root(selector("hoglin",
				doCharge(),
				behavior::meleeAttack(mob, actor::Target(), 0),
				behavior::moveTo(actor::Target(), Relative(ChaseSpeed)),
				behavior::defaultRoam()
			)),
			sequence("update-targets",
				every(0.5s,
					set(actor::Target(), actor::ClosestEnemy())
				),
				every(3s,
					set(location::Anchor(), location::Self())
				)			
			)
		);
	}
}}}
