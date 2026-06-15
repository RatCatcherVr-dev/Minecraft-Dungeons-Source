#pragma once

#include "lovika/QuadrantAngle.h"
#include "lovika/BlockRegionTransform.h"
#include "lovika/world/level/LevelGenRandom.h"

enum class FitType {
	None,
	OnlyNow,
	OnlyRotated,
	Always
};

struct BlockCuboid;

FitType createFitType(bool fitsNow, bool fitsRotated);
FitType fitsXZ(const BlockCuboid&, RegionSize);

TOptional<QuadrantAngle> randomQuadrantAngle(FitType, LevelGenRandom&);
