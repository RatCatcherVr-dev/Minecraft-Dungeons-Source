/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"
#include "Dimension.h"

#include "world/level/Level.h"
#include <vector>

const std::string dimensionFileIds[] = {
	"Overworld",
	"Undefined"
};

Dimension::Dimension(Level& level, DimensionId dimId, Height height)
	: mLevel(level)
	, mSeaLevel(63)
	, mId(dimId)
	, mSkyDarken(Brightness::MIN)
	, mHeight(height)
{
	for (size_t i = 0; i < mBrightnessRamp.size(); ++i) {
		mBrightnessRamp[i] = static_cast<float>(i) / mBrightnessRamp.size();
	}
}

bool Dimension::showSky() const {
	return true;
}

bool Dimension::isDay() const {
	return mSkyDarken < 4;
}

float Dimension::getTimeOfDay(float a) const {
	//either use the real time, or the time stopped
	return getTimeOfDay(mLevel.getTime(), a);
}

float Dimension::getSkyDarken(float a) const {
	float td = getTimeOfDay(a);

	float brightness = 1 - (cosf(td * PI * 2.f) * 2.f + 0.2f);
	brightness = 1.f - Math::clamp(brightness, 0.f, 1.f);
	return brightness * 0.8f + 0.2f;
}

Brightness Dimension::getSkyDarken() const {
	return mSkyDarken;
}

const float* Dimension::getBrightnessRamp() const {
	return mBrightnessRamp.data();
}
DimensionId Dimension::getId() const {
	return mId;
}

Level& Dimension::getLevel() const {
	return mLevel;
}

float Dimension::getTimeOfDay(int time, float a) const {
	int dayStep = (int)(time % Level::TICKS_PER_DAY);
	float td = (dayStep + a) / Level::TICKS_PER_DAY - 0.25f;
	if (td < 0) {
		td += 1;
	}
	if (td > 1) {
		td -= 1;
	}
	float tdo = td;
	td = 1 - (float)((Math::cos(td * PI) + 1) / 2);
	td = tdo + (td - tdo) / 3.0f;
	return td;
}

bool Dimension::isUltraWarm() const {
	return mUltraWarm;
}

bool Dimension::hasCeiling() const {
	return mHasCeiling;
}
