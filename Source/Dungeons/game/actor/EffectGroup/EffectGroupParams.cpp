#include "Dungeons.h"
#include "EffectGroupParams.h"


FEffectGroupParams::FEffectGroupParams() {
}

FEffectGroupParams::FEffectGroupParams(float individualSpawnRadius, bool spawnAtOldPos, bool debugVisuals)
	: IndividualSpawnRadius(individualSpawnRadius)
	, bSpawnAtOldPositionIfOverlap(spawnAtOldPos)
	, bDebugVisuals(debugVisuals) {
}