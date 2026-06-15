#pragma once

#include "game/actor/character/mob/MobCharacter.h"
#include "BiomineSpawnPoint.generated.h"

UCLASS()
class DUNGEONS_API ABiomineSpawnPoint : public AActor {
	GENERATED_BODY()

public:
	ABiomineSpawnPoint(const FObjectInitializer& objectInitializer);

	bool CanUse();
};