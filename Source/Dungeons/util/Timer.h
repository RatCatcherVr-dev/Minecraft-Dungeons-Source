/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once
#include <functional>

class Timer {
public:
	using MilliSecondsProvider = std::function<int()>;

	Timer(float ticksPerSecond, const MilliSecondsProvider&, unsigned maxTicksPerUpdate = 10);
	int getTicks() const;
	float getAlpha() const;
	float getTimeScale() const;
	float getTime() const;
	void setTimeScale(float timeScale);
	void advanceTime();
	void advanceTimeQuickly();
	void skipTime();
	void stepTick(int numSteps = 1);
	bool stepping() const;

private:
	MilliSecondsProvider mMillisecondsProvider;
	float mTicksPerSecond;
	int mTicks;
	float mAlpha;
	float mTimeScale;
	float mPassedTime;
	float mLastTimeSeconds;
	int mLastMs;
	int mLastMsSysTime;
	float mAdjustTime;
	int mSteppingTick;
	int mMaxTicksPerUpdate;
};