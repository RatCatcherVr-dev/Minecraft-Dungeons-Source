#pragma once
#include "CoreMinimal.h"
#include "EffectGroupParams.generated.h"

USTRUCT(BlueprintType)
struct DUNGEONS_API FEffectGroupParams {
	GENERATED_BODY()

	FEffectGroupParams();
	FEffectGroupParams(float individualSpawnRadius, bool spawnAtOldPos, bool debugVisuals);

	// If within this radius of a spawned effect no new effect will spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float IndividualSpawnRadius = 300.f;

	// If this is true the closest effect will be reset, if false the closest effect will move to the new location
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSpawnAtOldPositionIfOverlap = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDebugVisuals = false;
};

