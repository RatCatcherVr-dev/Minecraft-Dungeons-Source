/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "util/Math.h"

class FloatRange {
public:
	FloatRange(float max);
	FloatRange(float min, float max);

	static FloatRange minRange(float min);
	static const FloatRange unit;

	float min() const {
		return mMin;
	}

	float range() const {
		return (mMax - mMin);
	}

	float max() const {
		return mMax;
	}

	float lerp(float a) const {
		return mMin + a * (mMax - mMin);
	}

	float clampedLerp(float a) const {
		return lerp(Math::clamp(a, 0.0f, 1.0f));
	}

	float fractionAt(float value) const {
		return (mMax == mMin)? 0 : (value - mMin) / range();
	}

	float clampedFractionAt(float value) const {
		return Math::clamp(fractionAt(value), 0.0f, 1.0f);
	}

	bool inRange(float value) const {
		return value >= mMin && value <= mMax;
	}

	bool inRangeSquared(float value) const {
		return value >= (mMin*mMin) && value <= (mMax*mMax);
	}

	bool inExclusiveRange(float value) const {
		return value > mMin && value < mMax;
	}

	bool inExclusiveRangeSquared(float value) const {
		return value > (mMin*mMin) && value < (mMax*mMax);
	}

	float random(class Random* = nullptr) const;
private:
	float mMin, mMax;
};

FloatRange operator+(const FloatRange&, float offset);
FloatRange operator-(const FloatRange&, float offset);
FloatRange operator*(const FloatRange&, float times);

FloatRange mergeRanges(const FloatRange&, const FloatRange&);
