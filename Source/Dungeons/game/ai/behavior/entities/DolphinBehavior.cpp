#include "Dungeons.h"
#include "AnimalBehavior.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/FleeBehavior.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/provider/Locations.h"
#include "../../../component/BehaviorOptionsComponent.h"
#include "../../../component/DolphinBehaviorComponent.h"
#include "../../../actor/character/mob/MobCharacter.h"
#include "../MoveBehavior.h"
#include "../../task/attack/MeleeAttack.h"
#include "../../task/PlayAnimation.h"

namespace bt { namespace behavior { namespace entities {

	BehaviorTuple createDolphin(class AMobCharacter& mob, const UBehaviorOptionsComponent& options)
	{
		UDolphinBehaviorComponent* DolphinComponent = mob.FindComponentByClass<UDolphinBehaviorComponent>();

		return BehaviorTuple(
			selector(
				sequence("ChestDistanceCheck",
					predicate([=](StateRef state) { return DolphinComponent->GetState() == EMobGroupAttackedState::Normal && DolphinComponent->IsNearChest(); }),
					playAnimation(options.Get("NearChest"), true)
				),

				sequence("DolphinAttack",
					predicate(!actor::HasRecentlyAttacked(0.3s) && [=](StateRef state) { return DolphinComponent->GetState() == EMobGroupAttackedState::Agressive; }),
					behavior::chase([=](StateRef state) { return DolphinComponent->GetLastAttacker(); }, options.Get("ChaseMinDistance", 3000), Relative(options.Get("ChaseSpeedMultiplier", 1.2f))),
					make_unique<UMeleeAttack>([=](StateRef state) { return DolphinComponent->GetLastAttacker(); }, false)
				),

				sequence("Roam",
					predicate([=](StateRef state) { return DolphinComponent->GetState() == EMobGroupAttackedState::Normal && DolphinComponent->GetNearestTreasureChest() == nullptr; }),
					behavior::defaultRoam()
				)
			),
			every(5s,
				sequence("SetAnchor",
					ifElse(predicate([=](StateRef state) { return DolphinComponent->GetState() == EMobGroupAttackedState::Normal && DolphinComponent->GetNearestTreasureChest() != nullptr && DolphinComponent->IsNearPlayer(); }),
					behavior::moveTo([=](StateRef state) {
						FVector ChestLocation = DolphinComponent->GetNearestTreasureChest()->GetActorLocation();
						return ChestLocation;
					},
					Relative(options.Get("MoveToChestSpeedMultiplier", 1.2f))),
					set(location::Anchor(), location::Self())
			)))
		);
	}

}}}
