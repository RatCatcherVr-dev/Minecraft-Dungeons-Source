#pragma once

#include "Engine/EngineTypes.h"

namespace DungeonsDebug {
	void drawDebugWedge(UWorld* world, FVector fromLocation, FVector direction, float minDistUnits, float maxDistUnits, float maxAngleRadians, float maxZDiff, float coneOffset, FColor const& color);
}