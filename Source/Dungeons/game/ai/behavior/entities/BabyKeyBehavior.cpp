#include "Dungeons.h"
#include "BabyKeyBehavior.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/FleeBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/task/PlayAnimation.h"
#include "game/ai/bt/decorator/BtDropFor.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/condition/BtPredicate.h"
#include "util/FloatRange.h"
#include "game/component/PickupComponent.h"

namespace bt { namespace behavior { namespace entities {

BehaviorTuple createBabyKey(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
	static const auto pickupLocationProvider = [](StateRef state) {
		const auto pickupComponent = state.owner->FindComponentByClass<UPickupComponent>();
		return pickupComponent->StoredLocation.GetValue();
	};

	//D11.PS changed auto to Pred
	static const Pred pickupLocationPredicate = [](StateRef state) {
		const auto pickupComponent = state.owner->FindComponentByClass<UPickupComponent>();
		return pickupComponent->StoredLocation.IsSet();
	};

	const auto sleeping = sharedRef(false);

	return BehaviorTuple(
		selector("BabyKey",
			greedySequence(
				predicate(
					!locator::IsInRange(actor::Target(), FloatRange { options.Get("SleepSafeDistance", 500.f) }) &&
					(!pickupLocationPredicate || locator::IsInRange(pickupLocationProvider, FloatRange { options.Get("HomeAcceptanceDistance", 500.f) }))
				),
				dropFor(options.Get("CalmdownDuration", 5s),
					sequence(
						onStart(set(sleeping, value(true))),
						playAnimation(options.Get("Sleeping"), true)
					)
				),
				onStop([&, sleeping](StateRef state) {
					if (sleeping(state)) {
						state.owner->MulticastPlayAnimationAsDynamicMontage(options.Get("Wakeup"), FName(TEXT("FullBody")));
						sleeping(state, false);
					}
				})
			),
			greedySequence(
				predicate(pickupLocationPredicate),
				behavior::moveTo(pickupLocationProvider, Relative(1))
			),
			defaultFleeFrom(actor::Target(), options.Get("SleepSafeDistance", 500.f))
		),
		every(0.5s, set(actor::Target(), actor::ClosestPlayer()))
	);
}

}}}
