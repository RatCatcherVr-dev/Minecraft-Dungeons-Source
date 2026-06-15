#include "Dungeons.h"
#include "game/util/ActorQuery.h"
#include "BiomineSpawnPoint.h"

ABiomineSpawnPoint::ABiomineSpawnPoint(const FObjectInitializer& objectInitializer) : Super(objectInitializer) {

}

bool ABiomineSpawnPoint::CanUse() {
	const auto IsBiomine = [&](AActor* mob) {
		return Cast<AMobCharacter>(mob) && Cast<AMobCharacter>(mob)->EntityType == EntityType::Biomine;
	};

	float closestBiomineDistanceSquared;
	if (AMobCharacter* mineActor = Cast<AMobCharacter>(actorquery::getClosestMob(this, 100.0f, closestBiomineDistanceSquared, IsBiomine))) {
		if (UHealthComponent* healthComponent = mineActor->GetHealthComponent()) {
			return healthComponent->IsNotAlive();
		}
		return false;
	}
	return true;
}
