#include "Dungeons.h"
#include "ActorStats.h"
#include "game/component/HealthComponent.h"
#include "game/component/TurnComponent.h"
#include "game/component/RangedAttackComponent.h"

namespace bt { namespace actorStats {

Provider<float> healthFraction(const actor::Provider& provider) {
	return [provider](StateRef state) {
		if (const auto actor = provider(state)) {
			if (const auto healthComponent = actor->FindComponentByClass<UHealthComponent>()) {
				return healthComponent->GetCurrentHealthPercentage();
			}
		}
		return 1.f;
	};	
}

Pred isTurning(const actor::Provider& provider) {
	return [provider](StateRef state) {
		if (const auto actor = provider(state)) {
			if (const auto turnComponent = actor->FindComponentByClass<UTurnComponent>()) {
				return turnComponent->IsTurning();
			}
		}
		return false;
	};
}

}}
