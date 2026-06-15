#include "Dungeons.h"
#include "TurtleBehavior.h"
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
#include "game/ai/behavior/FleeBehavior.h"
#include "game/ai/provider/Actors.h"
#include "../RangedAttackBehavior.h"
#include "game/Enchantments/Blind.h"
#include "game/component/MobGroupBehaviorComponent.h"

namespace bt {
	namespace behavior {
		namespace entities {
			BehaviorTuple createTurtle(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {

				UMobGroupBehaviorComponent* MobGroupComponent = mob.FindComponentByClass<UMobGroupBehaviorComponent>();

				return BehaviorTuple(
					selector("Turtle",
						sequence(
							predicate(actor::IsRecentlyDamagedFromAttack(3.0s)),
							[=](StateRef state) { MobGroupComponent->OnMobWasAttacked(actor::LastAttacker()(state)); },
							behavior::defaultFleeFrom(actor::LastAttacker())
						),

						sequence("TurtleAttacked",
							predicate(actor::IsRecentlyDamagedFromAttack(1s)),
							[=](StateRef state) { MobGroupComponent->OnMobWasAttacked(actor::LastAttacker()(state)); }
						),

						sequence("TurtleFlee",
							predicate(!actor::HasRecentlyAttacked(0.3s) && [=](StateRef state) { return MobGroupComponent && MobGroupComponent->GetState() == EMobGroupAttackedState::Flee; }),
							behavior::defaultFleeFrom([=](StateRef state) { return MobGroupComponent->GetLastAttacker(); }, 700.0f)
						),
						behavior::defaultRoam()
					));
			}
		}
	}
}
