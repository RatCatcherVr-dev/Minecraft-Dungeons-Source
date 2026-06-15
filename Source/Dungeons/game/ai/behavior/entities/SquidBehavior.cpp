#include "Dungeons.h"
#include "SquidBehavior.h"
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

namespace bt {
	namespace behavior {
		namespace entities {
			BehaviorTuple createSquid(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {

				float EffectDuration = options.Get("BlindCloudDuration", 10.0f);
				auto BlindCloud = options.GetClass("InkCloud").Get();
				auto BlindEffect = options.GetEffect("BlindGameplayEffect");

				const auto bCreatedBlindCloud = makeSharedRef<bool>(false);

				auto SpawnBlindCloud = [=]() -> bt::Action {
					return[=](bt::StateRef state) {
						UBlindCloudSpawnHelper::SpawnBlindCloud(BlindCloud, BlindEffect, state.owner, state.owner->GetActorLocation(), EffectDuration, true);
						set(bCreatedBlindCloud, value(true));
						return true;
					};
				};

				return BehaviorTuple(
					selector("squid",
						greedySequence(
							predicate(actor::IsRecentlyDamagedFromAttack(0.65s) && equals(bCreatedBlindCloud, value(false))),
							SpawnBlindCloud(),
							behavior::defaultFleeFrom(actor::LastAttacker())
						),
						sequence(
							predicate(actor::IsRecentlyDamagedFromAttack(5.0s)),
							behavior::defaultFleeFrom(actor::LastAttacker())
						),
						parallel(
							every(3.0s, set(bCreatedBlindCloud, value(false))),
							behavior::defaultRoam()
						)
					)
				);
			}
		}
	}
}


