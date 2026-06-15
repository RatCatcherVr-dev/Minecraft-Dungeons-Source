/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "CommonTypes.h"

class Level;
class Vec3;

class Dimension {
	friend class Level;
public:
	Dimension(Level& level, DimensionId dimId, Height height);
	virtual ~Dimension() {}

	virtual std::string getName() const = 0;

	virtual bool showSky() const;
	virtual bool isDay() const;
	float getTimeOfDay(float a) const;
	float getSkyDarken(float a) const;
	Height getHeight() const {
		return mHeight;
	}

	DimensionId getId() const;

	Level& getLevel() const;

	bool isUltraWarm() const;

	bool hasCeiling() const;
	
	Brightness getSkyDarken() const;

	Brightness getSkyLight() const {
		return Brightness();
	}

	const float* getBrightnessRamp() const; 
protected:
	virtual float getTimeOfDay(int time, float a) const;

	Level& mLevel;

	Height mSeaLevel;
private:
	DimensionId mId;
	bool mUltraWarm = false;
	bool mHasCeiling = false;
	Brightness mSkyDarken;
	Height mHeight;

	std::array<float, 16> mBrightnessRamp;
};
