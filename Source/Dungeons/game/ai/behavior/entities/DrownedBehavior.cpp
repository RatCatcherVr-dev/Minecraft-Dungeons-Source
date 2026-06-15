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

class UBehaviorOptionsComponent;

namespace bt {
	namespace behavior {
		namespace entities {
			BehaviorTuple createDrowned(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
				return BehaviorTuple(
					selector("Drowned",

						behavior::meleeAttack(mob),

						every(1.21s,
							behavior::chaseInOffensiveRange(mob, actor::Target(), Relative(0.4f))
						),

						behavior::chaseIfAttacked(mob, Relative(0.5f)),
						behavior::chaseIfWarned(mob, Relative(0.5f)),
						behavior::defaultRoam()
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
