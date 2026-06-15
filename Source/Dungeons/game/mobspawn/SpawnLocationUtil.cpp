#include <CollisionQueryParams.h>
#include "game/util/LocationQuery.h"
#include "game/actor/character/BaseCharacter.h"
#include <GameFramework/Actor.h>
#include <NavigationSystem.h>


namespace game { namespace mobspawn {
FVector RandomLocationAround(const AActor* actor, float radius) {
	const auto actorLocation = actor->GetActorLocation();

	FNavLocation result;
	if (FNavigationSystem::GetCurrent<UNavigationSystemV1>(actor->GetWorld())->GetRandomReachablePointInRadius(actorLocation, radius, result)) {
		return result.Location;
	}

	return actorLocation;
}

}}