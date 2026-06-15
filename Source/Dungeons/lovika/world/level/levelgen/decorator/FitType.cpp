#include "Dungeons.h"
#include "FitType.h"
#include "lovika/BlockCuboid.h"

FitType createFitType(bool fitsNow, bool fitsRotated) {
	if (fitsNow && fitsRotated) { return FitType::Always; }
	if (fitsNow) { return FitType::OnlyNow; }
	if (fitsRotated) { return FitType::OnlyRotated; }
	return FitType::None;
}

FitType fitsXZ(const BlockCuboid& area, RegionSize size) {
	return createFitType(area.fits(size.x, 0, size.z), area.fits(size.z, 0, size.x));
}

TOptional<QuadrantAngle> randomQuadrantAngle(FitType fitType, LevelGenRandom& rnd) {
	switch (fitType) {
	case FitType::Always: return (QuadrantAngle) rnd.nextInt(4);
	case FitType::OnlyNow: return rnd.nextBoolean() ? QuadrantAngle::D0 : QuadrantAngle::D180;
	case FitType::OnlyRotated: return rnd.nextBoolean() ? QuadrantAngle::D90 : QuadrantAngle::D270;
	default: return {};
	}
}
