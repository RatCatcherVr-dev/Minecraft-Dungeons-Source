#include "Dungeons.h"
#include "Transform.h"
#include "game/actor/character/mob/MobCharacter.h"

namespace bt { namespace transform {

Provider<float> distanceTo(const locator::Provider& targetProvider) {
	return [=](StateRef state) {
		if (const auto maybeTarget = targetProvider(state).GetLocationLike()) {
			return FVector::Dist2D(state.owner->GetActorLocation(), maybeTarget.GetValue());
		}
		return 0.f;
	};
}

}}
