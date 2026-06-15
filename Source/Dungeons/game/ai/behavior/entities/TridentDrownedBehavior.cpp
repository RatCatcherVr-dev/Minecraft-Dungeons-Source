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
#include "game/ai/behavior/RangedAttackBehavior.h"
#include "game/component/RangedAttackComponent.h"
#include "game/ai/behavior/FleeBehavior.h"

class UBehaviorOptionsComponent;

namespace bt {
	namespace behavior {
		namespace entities {
			BehaviorTuple createTridentDrowned(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
				URangedAttackComponent& rangedComponent = *mob.FindComponentByClass<URangedAttackComponent>();
				
				return BehaviorTuple(root(
					selector("TridentDrowned",
						uninterruptibleSequence(
							startPredicate(locator::IsInRange(actor::Target(), FloatRange(options.Get("MinAttackDistance", 500.f), options.Get("RangedAcquireTargetRangeUnits", 1000.f)))),
							rangedAttack(
							mob,
							0.0f,
							options.Get("RangedAcquireTargetRangeUnits", 1000.f),
							options.Get("RangedLoseTargetRangeUnits", 1200.f),
							actor::Target()
							)
						),
						sequence("melee-attack",
							isInRange(actor::Target(), 500),
							selector(
								behavior::meleeAttack(mob),
								behavior::chargeTo(actor::Target(), Relative(1.7f), 1.5s, 5s, Relative(0.6f)),
								behavior::moveTo(actor::Target(), Relative(0.5f))
							)
						),
						behavior::chase(actor::Target(), FloatRange(rangedComponent.GetAttackRange(), options.Get("RangedAcquireTargetRangeUnits", 1000.f)), Relative(options.Get("ChaseSpeedMultiplier", .5f))),
						behavior::chaseIfAttacked(mob, Relative(options.Get("ChaseIfAttackedSpeedMultiplier", .6f))),
						behavior::chaseIfWarned(mob, Relative(options.Get("ChaseIfWarnedSpeedMultiplier", .6f))),
						behavior::defaultRoam()
					)
					),

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

		}
	}
}
