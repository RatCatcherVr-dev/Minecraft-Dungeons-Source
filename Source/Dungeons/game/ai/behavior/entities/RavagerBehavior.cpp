#include "Dungeons.h"
#include "RavagerBehavior.h"
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

	BehaviorTuple createRavager(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {

		const auto optChaseSpeed = options.Get("chase-speed", 1.5f);

		const auto optChargeMinDistance = options.Get("charge-min-distance", 400.0f);
		const auto optChargeMaxDistance = options.Get("charge-max-distance", 2000.0f);
		const auto optChargeSpeed = options.Get("charge-speed", 1.8f);
		const auto optChargeCooldown = options.Get("charge-cooldown", 8.0s);
		const auto optChargeAttackDuration = options.Get("charge-attackDuration", 2.0s);
		const auto optChargeMaxDuration = options.Get("charge-max-duration", 1.75s);

		const auto optRoarThreshold = options.Get("roar-health-threshold", 0.5f);
		const auto optRoarCooldown = options.Get("roar-cooldown", 3.0f);

		const auto doRoar = [&]() {
			return uninterruptibleSequence("roar",
				startPredicate(
					lessThan(actorStats::healthFraction(), value(optRoarThreshold)) && 
					actor::IsRecentlyDamagedFromAttack(bt::Seconds(optRoarCooldown))
				),
				minTimeBetweenStopAndStart(bt::Duration(bt::Seconds(optRoarCooldown))),
				onStart(focus::Set(actor::Target(), EAIFocusPriority::Attacking)),
				onStop(focus::Clear(EAIFocusPriority::Attacking)),
				sequence(
					playAnimation(options.Get("Roar")),
					behavior::aoeAttack(mob, FName("AoeRoar")),
					delay(0.5s)
				)
			);
		};
		
		const auto doCharge = [&]() {
			return uninterruptibleSequence("charge-attack",
				startPredicate(
					locator::IsInRange(actor::Target(), optChargeMaxDistance) &&
					!locator::IsInRange(actor::Target(), optChargeMinDistance) &&
					actor::IsAlive(actor::Target()) &&
					actor::IsInLineOfSight(actor::Target())
				),
				minTimeBetweenStopAndStart(optChargeCooldown),
				onStart(focus::Set(actor::Target(), EAIFocusPriority::Attacking)),
				onStop(focus::Clear(EAIFocusPriority::Attacking)),
				chain("charge-or-hit",
					playAnimation(options.Get("ChargeStart")),
					sequence("charge",
						playAnimation(options.Get("Charge"), true),
						behavior::chargeTo(actor::Target(), Relative(optChargeSpeed), optChargeMaxDuration, 5.0s, Relative(optChargeSpeed), 150.0f)
					),
					sequence("charge-melee",
						behavior::aoeAttack(mob, FName("AoeCharge")),
						delay(optChargeAttackDuration)
					)								
				)

			);
		};

		return BehaviorTuple(
			root(selector("ravager",
				doCharge(),
				doRoar(),
				behavior::meleeAttack(mob, actor::Target()),
				behavior::moveTo(actor::Target(), Relative(optChaseSpeed)),
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
