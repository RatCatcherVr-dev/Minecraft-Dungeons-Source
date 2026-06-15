#include "Dungeons.h"
#include "PetBehavior.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/provider/Actors.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/condition/BtProbability.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/provider/BtRandom.h"
#include "game/ai/action/AnimActions.h"
#include "game/util/LocationQuery.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/Enchantments/PushVolumeResistance.h"

namespace bt { namespace behavior { namespace entities {

BehaviorTuple createPet(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
	static Random rnd;

	const auto rightHand = rnd.nextBoolean() ? -1.f : 1.f;

	if (auto ability = mob.GetAbilitySystemComponent()) {
		ability->ApplyGameplayEffectSpecToSelf(effects::CreateGameplayEffectSpec<UPushVolumeImmunityGameplayEffect>(ability));
	}

	const auto FollowTriggerDistance = options.Get("FollowTriggerDistance", 800.f);
	const auto AheadDistance = options.Get("AheadDistance", 300.f);
	const auto SideDistance = options.Get("SideDistance", 150.f);
	const auto RoamInterval = options.Get("RoamInterval", 3.s);

	return BehaviorTuple(
		root(selector("Pet",
			uninterruptible(sequence(
				startPredicate(
					!locator::IsInRange(actor::Master(), { FollowTriggerDistance })
				),
				follow(
					[
						rightHand,
						aheadDistance = AheadDistance,
						sideDistance = SideDistance
					](StateRef state) {
						if (const auto master = state.owner->GetMaster()) {
							if (locationquery::isInFrontOf(master, state.owner->GetActorLocation())) {
								return master->GetActorLocation() + master->GetActorForwardVector() * aheadDistance;
							} else {
								return master->GetActorLocation() + master->GetActorForwardVector().RotateAngleAxis(60.f * rightHand, FVector::UpVector) * sideDistance;
							}
						} else {
							return state.owner->GetActorLocation();
						}
					}
				),				
				onStop(
					set(
						location::Anchor(),
						[](StateRef state) { return state.owner->GetActorLocation() + state.owner->GetActorForwardVector() * 250.f; }
					)
				)
			)),
			parallel(
				behavior::smoothRoam(RoamInterval),
				sequence(
					every(1.s, selector(
						sequence(
							predicate(actor::IsInFront(actor::Master()) && random::randomBool(.9f)),
							anim::SetLookAtTarget(actor::Master())
						),
						anim::SetLookAtTarget(actor::Master())
					)),
					onStop(anim::ClearLookAtTarget())
				)
			)
		)),
		{}
	);
}

}}}
