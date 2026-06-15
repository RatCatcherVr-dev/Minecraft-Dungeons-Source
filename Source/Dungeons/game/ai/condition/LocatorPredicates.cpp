#include "Dungeons.h"
#include "LocatorPredicates.h"
#include "util/FloatRange.h"
#include "game/actor/character/mob/MobCharacter.h"

namespace bt { namespace locator {

Pred IsInRange(Provider locatorProvider, FloatRange range) {
	return [locator = std::move(locatorProvider), range](StateRef state) {
		const auto loc = locator(state);
		if (auto pt = loc.GetLocationLike()) {
			auto ownerCapsule = state.owner->FindComponentByClass<UCapsuleComponent>();
			if (loc.HasActor()) {
				const auto* actorCapsule = loc.actor->FindComponentByClass<UCapsuleComponent>();
				const auto capsuleOffset = (actorCapsule ? actorCapsule->GetScaledCapsuleRadius() : 0.f) + (ownerCapsule ? ownerCapsule->GetScaledCapsuleRadius() : 0.f);
				return range.inRange(FVector::Dist(pt.GetValue(), state.owner->GetActorLocation()) - capsuleOffset);
			}
			return range.inRangeSquared(FVector::DistSquared(pt.GetValue(), state.owner->GetActorLocation()));
		}
		return false;
	};
}

}}
