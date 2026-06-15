#include "Dungeons.h"
#include "Move.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/GameBP.h"
#include <NavigationSystem.h>

namespace bt { namespace move {

FAIMoveRequest defaultRequest() {
	FAIMoveRequest moveRequest;
	moveRequest.SetUsePathfinding(true);
	moveRequest.SetAllowPartialPath(false);
	moveRequest.SetProjectGoalLocation(false);
	//moveRequest.SetNavigationFilter(*FilterClass ? FilterClass : DefaultNavigationFilterClass);
	moveRequest.SetAcceptanceRadius(100.0f);
	moveRequest.SetReachTestIncludesAgentRadius(true);
	moveRequest.SetCanStrafe(true);
	return moveRequest;
}

Provider Actor(const actor::Provider& provider) {
	return Actor(provider, defaultRequest());
}

Provider Actor(const actor::Provider& provider, const FAIMoveRequest& requestTemplate) {
	return [=](StateRef state) {
		auto request = requestTemplate;
		request.SetGoalActor(provider(state));
		return request;
	};
}

Provider Location(const location::Provider& provider) {
	return [=](StateRef state) {
		FAIMoveRequest request = defaultRequest();
		request.SetGoalLocation(provider(state));
		return request;
	};
}

Provider Forward(const bt::Provider<float>& offset) {
	return [=](StateRef state) {
		FAIMoveRequest request = defaultRequest();
		request.SetGoalLocation(state.owner->GetActorLocation() + state.owner->GetActorForwardVector() * offset(state));
		return request;
	};
}

Provider RandomLocationAround(const locator::Provider& provider, float radius) {
	return [=](StateRef state) {
		if (const auto maybeLocation = provider(state).GetLocationLike()) {
			
			//D11.SC stolen from with ARecastNavMesh
			const float RandomAngle = 2.f * PI * FMath::FRand();
			const float U = FMath::FRand() + FMath::FRand();
			const float RandomRadius = radius * (U > 1 ? 2.f - U : U);
			const FVector RandomOffset(FMath::Cos(RandomAngle) * RandomRadius, FMath::Sin(RandomAngle) * RandomRadius, 0);
			FVector RandomLocationInRadius = maybeLocation.GetValue() + RandomOffset;

			FAIMoveRequest request = defaultRequest();
			request.SetGoalLocation(RandomLocationInRadius);
			request.SetProjectGoalLocation(true);
			request.SetAllowPartialPath(true);

			return request;
			
		}
		
		return FAIMoveRequest {};
	};
}

Provider LocationAwayFrom(const locator::Provider& provider, float distance /*= 500.0f*/, float radius /*= 200.0f*/) {
	return [=](StateRef state) {
		auto center = state.owner->GetActorLocation();
		if (auto opposite = provider(state).GetLocationLike()) {
			auto delta = center - opposite.GetValue();
			delta.Normalize();
			auto targetCenter = center + delta * distance;

			FNavLocation dest;
			if (FNavigationSystem::GetCurrent<UNavigationSystemV1>(&state.world())->GetRandomPointInNavigableRadius(targetCenter, radius / 3, dest)) {
				FAIMoveRequest request = defaultRequest();
				request.SetGoalLocation(dest.Location);
				return request;
			}
			if (FNavigationSystem::GetCurrent<UNavigationSystemV1>(&state.world())->GetRandomPointInNavigableRadius(targetCenter, radius, dest)) {
				FAIMoveRequest request = defaultRequest();
				request.SetGoalLocation(dest.Location);
				return request;
			}

		}
		return FAIMoveRequest();
	};
}

Provider From(const locator::Provider& provider) {
	return [=](StateRef state) {
		if (const auto maybeLocation = provider(state).GetLocationLike()) {
			FAIMoveRequest request = defaultRequest();
			request.SetGoalLocation(maybeLocation.GetValue());
			return request;
		}

		return FAIMoveRequest {};
	};
}

Provider withSettings(const Provider& provider, const FAIMoveRequest& requestTemplateWithoutGoal) {
	return [=](StateRef state) {
		FAIMoveRequest request = requestTemplateWithoutGoal;
		FAIMoveRequest target = provider(state);
		if (target.IsMoveToActorRequest()) {
			request.SetGoalActor(target.GetGoalActor());
		} else if (target.IsValid()) {
			request.SetGoalLocation(target.GetGoalLocation());
		}
		return request;
	};
}

}}
