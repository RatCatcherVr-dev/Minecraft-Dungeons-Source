#include "Dungeons.h"
#include "AttackPredicates.h"
#include "game/component/AttackComponent.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/provider/Actors.h"

namespace bt { namespace attack {

Pred InAttackRange(const locator::Provider& target, UAttackComponent* component, bool allowNoTarget /* = false*/) {
	if (!component) {
		return [](StateRef state) { return false; };
	}
	return [=](StateRef state) {
		auto locator = target(state);
		if (auto location = locator.GetLocationLike()) {
			const auto* ownerCapsule = state.owner->FindComponentByClass<UCapsuleComponent>();
			const auto* targetCapsule = state.params().target.actor.IsValid() ? state.params().target.actor->FindComponentByClass<UCapsuleComponent>() : nullptr;
			const auto targetPos = location.GetValue();
			const auto ownerPos = state.owner->GetActorLocation();
			const auto targetFeet = FVector{ FVector2D(targetPos), targetPos.Z - (targetCapsule ? targetCapsule->GetScaledCapsuleHalfHeight() : 0.f) };
			const auto ownerFeet = FVector{ FVector2D(ownerPos), ownerPos.Z - (ownerCapsule ? ownerCapsule->GetScaledCapsuleHalfHeight() : 0.f) };
			const auto capsuleOffset = (ownerCapsule ? ownerCapsule->GetScaledCapsuleRadius() : 0.f) + (targetCapsule ? targetCapsule->GetScaledCapsuleRadius() : 0.f);
			return ((FVector::Dist(targetFeet, ownerFeet) - capsuleOffset) < component->GetAttackRange());
		} else {
			return allowNoTarget;
		}
	};
}

Pred InAttackOrientation(const actor::Provider& target, UAttackComponent* component, bool allowNoTarget /* = false*/) {
	if (!component) {
		return [](StateRef state) { return false; };
	}
	return [=](StateRef state) {
		auto locator = target(state);
		if (locator) {
			return component->InAttackOrientation(locator);
		}
		else {
			return allowNoTarget;
		}
	};
}

Pred IsAttackInProgress(UAttackComponent* component) {
	if (!component) {
		return [](StateRef state) { return false; };
	}
	return [=](StateRef state) {
		return component->IsAttackInProgress();
	};
}


Pred InAttackRangeOrAttacking(const locator::Provider& target, UAttackComponent* component) {
	if (!component) {
		return [](StateRef state) { return false; };
	}
	return [=](StateRef state) {
		auto locator = target(state);
		if (auto location = locator.GetLocationLike()) {
			return FVector::DistSquared(location.GetValue(), state.owner->GetActorLocation()) < component->GetAttackRangeSquared() || component->IsAttackInProgress();
		}
		return false;
	};
}

}}
