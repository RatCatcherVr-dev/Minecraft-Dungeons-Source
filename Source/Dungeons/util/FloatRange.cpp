/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"
#include "FloatRange.h"
#include "SharedRandom.h"
#include <limits>

FloatRange::FloatRange(float max)
	: FloatRange(0, max) {
}

FloatRange::FloatRange(float min, float max)
	: mMin(min)
	, mMax(max) {
}

FloatRange FloatRange::minRange(float min) {
	return FloatRange(min, std::numeric_limits<float>::max());
}

const FloatRange FloatRange::unit{ 0, 1 };

float FloatRange::random(Random* rnd /*= nullptr*/) const {
	return Util::thisOrSharedRandom(rnd)->nextFloat(mMin, mMax);
}

FloatRange operator+(const FloatRange& range, float offset) {
	return{ range.min() + offset, range.max() + offset };
}

FloatRange operator-(const FloatRange& range, float offset) {
	return{ range.min() - offset, range.max() - offset };
}

FloatRange operator*(const FloatRange& range, float times) {
	return{ range.min() * times, range.max() * times };
}

FloatRange mergeRanges(const FloatRange& a, const FloatRange& b) {
	return { FMath::Min(a.min(), b.min()), FMath::Max(a.max(), b.max()) };
}
