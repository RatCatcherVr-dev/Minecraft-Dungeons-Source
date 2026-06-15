#include "Dungeons.h"
#include "Locators.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/mob/MobParams.h"
#include "game/util/LocationQuery.h"
#include <NavigationSystem.h>
#include "game/Conversion.h"
#include "game/GameBP.h"
#include "lovika/world/level/terrain/Terrain.h"

namespace bt { namespace locator {

FLocator Target::operator()(StateRef state) const {
	return state.params().target;
}

void Target::operator()(StateRef state, FLocator locator) const {
	state.params().target = locator;
}

LastValid::LastValid(const Provider& provider)
	: provider(provider) {
}

FLocator LastValid::operator()(StateRef state) const {
	if (provider) {
		auto locator = provider(state);
		if (locator.IsValid()) {
			lastValidLocationLocator = locator.GetLocationLike().GetValue();
			return locator;
		}
	}
	return lastValidLocationLocator;
}

Provider RandomReachablePointAround(const Provider& provider, float radius) {
	return [=](StateRef state) -> FLocator {
		auto center = provider(state).GetLocationLike();
		if (!center) {
			return {};
		}

		FNavLocation dest;
		if (!FNavigationSystem::GetCurrent<UNavigationSystemV1>(&state.world())->GetRandomReachablePointInRadius(center.GetValue(), radius, dest)) {
			return {};
		}

		const auto terrain = state.game().GetTerrain();
		const auto cellType = terrain->getType(conversion::ueToTerrain(dest.Location));
		if (!cellType.isReachable()) {
			return {};
		}

		return { dest.Location };
	};
}

Provider RandomReachablePointAroundSmooth(const Provider& centerProvider, const bt::Provider<AActor*>& ownerProvider, float radius) {
	return [=](StateRef state) -> FLocator {
		const auto center = centerProvider(state).GetLocationLike();
		if (!center) {
			return {};
		}

		const auto* owner = ownerProvider(state);
		if (owner == nullptr) {
			return {};
		}
		
		const auto dotProduct = (center.GetValue() - owner->GetActorLocation()).GetSafeNormal2D() | owner->GetActorForwardVector();
		const auto angle = dotProduct > .3f ? HALF_PI * .5f
			: dotProduct > -3.f ? HALF_PI
			: PI * .9f;

		const auto triesMax { 10 };
		for (auto i = 0; i < triesMax; i++) {
			FNavLocation dest;
			if (!FNavigationSystem::GetCurrent<UNavigationSystemV1>(&state.world())->GetRandomReachablePointInRadius(center.GetValue(), radius, dest)) {
				continue;
			}

			if (locationquery::isInFrontOf(owner, dest.Location, angle)) {
				return { dest.Location };
			}
		}

		return {};
	};
}

Provider RandomReachablePointAround(
	const Provider& provider,
	float radius,
	const std::function<bool (const FVector& target, const FVector& candidate)>& filter
) {
	return [=](StateRef state) -> FLocator {
		const auto maybeCenter = provider(state).GetLocationLike();
		if (!maybeCenter) {
			return {};
		}

		const auto center = maybeCenter.GetValue();

		FNavLocation dest;
		const auto triesMax { 10 };
		for (auto i = 0; i < triesMax; i++) {
			const bool valid = FNavigationSystem::GetCurrent<UNavigationSystemV1>(&state.world())->GetRandomReachablePointInRadius(center, radius, dest);

			if (filter(center, dest.Location)) {
				return { dest.Location };
			}
		}

		return {};
	};
}

Provider RandomReachablePointAround(const Provider& provider, float radiusMin, float radiusMax) {	
	return RandomReachablePointAround(
		provider,
		radiusMax,
		[radiusMinSquared = FMath::Square(radiusMin)](const auto& center, const auto& candidate) {
			return FVector::DistSquared2D(center, candidate) > radiusMinSquared;
		}
	);
}

Provider Forward(float offset) {
	return [=](StateRef state) -> FLocator {
		return { state.owner->GetActorLocation() + state.owner->GetActorForwardVector() * offset };
	};
}

Provider Forward(const bt::Provider<AActor*>& provider, float offset) {
	return [=](StateRef state) -> FLocator {
		if (const auto target = provider(state)) {
			return { target->GetActorLocation() + target->GetActorForwardVector() * offset };
		}

		return {};
	};
}

Provider Side(const Provider& provider, float radius, bool clockwise) {
	return [=](StateRef state) -> FLocator {
		if (const auto maybeTarget = provider(state).GetLocationLike()) {
			const auto targetLocation = maybeTarget.GetValue();
			const auto ownerLocation = state.owner->GetActorLocation();

			const auto delta = ((ownerLocation - targetLocation).GetSafeNormal() * radius).RotateAngleAxis(clockwise ? 60.f : -60.f, FVector::UpVector);
			return targetLocation + delta;
		}

		return {};
	};
}

Provider Away(const Provider& provider, float distance/* = 500.f*/) {
	return [=](StateRef state) -> FLocator {
		if (const auto target = provider(state).GetLocationLike()) {
			const auto ownerLocation = state.owner->GetActorLocation();
			return { ownerLocation - (target.GetValue() - ownerLocation).GetSafeNormal() * distance };
		}

		return {};
	};
}

Provider From(const move::Provider& provider) {
	return [=](StateRef state) -> FLocator {
		auto request = provider(state);
		if (!request.IsValid()) {
			return {};
		}
		if (request.IsMoveToActorRequest()) {
			return{ request.GetGoalActor() };
		}
		return { request.GetDestination() };
	};
}

}}
