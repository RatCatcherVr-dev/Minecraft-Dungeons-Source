/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#include "Dungeons.h"
#include "Timer.h"
#include <algorithm>

Timer::Timer(float ticksPerSecond, const MilliSecondsProvider& millisecondsProvider, unsigned maxTicksPerUpdate)
	: mTicksPerSecond(ticksPerSecond)
	, mMillisecondsProvider(millisecondsProvider)
	, mTicks(0)
	, mAlpha(0)
	, mAdjustTime(1.0f)
	, mTimeScale(1.0f)
	, mPassedTime(0)
	, mSteppingTick(-1)
	, mMaxTicksPerUpdate(std::min(maxTicksPerUpdate, 1000000U))
{
	mLastMs = mMillisecondsProvider();
	mLastMsSysTime = mLastMs;
	mLastTimeSeconds = mLastMs / 1000.0f;
}

int Timer::getTicks() const {
	return mTicks;
}

float Timer::getAlpha() const {
	return mAlpha;
}

float Timer::getTimeScale() const {
	return mTimeScale;
}

float Timer::getTime() const {
	return mLastTimeSeconds;
}

void Timer::setTimeScale(float timeScale) {
	mTimeScale = Math::clamp(timeScale,0.0f,10.0f);
}

void Timer::advanceTime() {
	// --------------------------
	// Support stepping a single tick
	if (stepping())
	{
		// Done stepping? then we are paused.
		if (mSteppingTick == 0)
		{
			mTicks = 0;
			mAlpha = 0.0f;
			return;
		}

		mTicks = 1;
		--mSteppingTick;
		return;
	}
	// --------------------------

	auto nowMs = mMillisecondsProvider();
	auto passedMs = nowMs - mLastMs;
	auto msSysTime = nowMs;

	if (passedMs > 1000) {
		auto passedMsSysTime = msSysTime - mLastMsSysTime;
		if (passedMsSysTime == 0) {
			passedMs = passedMsSysTime = 1;
		}

		const float adjustTimeT = passedMs / (float)passedMsSysTime;
		mAdjustTime += (adjustTimeT - mAdjustTime) * 0.2f;

		mLastMs = nowMs;
		mLastMsSysTime = msSysTime;
	}
	if (passedMs < 0) {
		mLastMs = nowMs;
		mLastMsSysTime = msSysTime;
	}

	const float now = msSysTime / 1000.0f;
	float passedSeconds = (now - mLastTimeSeconds) * mAdjustTime;
	mLastTimeSeconds = now;

	if (passedSeconds < 0) {
		passedSeconds = 0;
	}
	if (passedSeconds > 1) {
		passedSeconds = 1;
	}
	//LOGI("passed s: %f\n", passedSeconds);

	mPassedTime += passedSeconds * mTimeScale * mTicksPerSecond;

	mTicks = (int)mPassedTime;
	mPassedTime -= mTicks;
	if (mTicks > mMaxTicksPerUpdate) {
		mTicks = mMaxTicksPerUpdate;
	}
	mAlpha = mPassedTime;
}

/**
* Advances time the max number of ticks per second.
*/
void Timer::advanceTimeQuickly() {
	const float passedSeconds = (float)mMaxTicksPerUpdate / (float)mTicksPerSecond;

	mPassedTime += passedSeconds * mTimeScale * mTicksPerSecond;
	mTicks = (int)mPassedTime;
	mPassedTime -= mTicks;
	mAlpha = mPassedTime;

	mLastMs = mMillisecondsProvider();
	mLastMsSysTime = mLastMs;
}

void Timer::skipTime() {
	const int nowMs = mMillisecondsProvider();
	int passedMs = nowMs - mLastMs;
	const int msSysTime = nowMs;

	if (passedMs > 1000) {
		int passedMsSysTime = msSysTime - mLastMsSysTime;
		if (passedMsSysTime == 0) {
			passedMs = passedMsSysTime = 1;
		}

		const float adjustTimeT = passedMs / (float)passedMsSysTime;
		mAdjustTime += (adjustTimeT - mAdjustTime) * 0.2f;

		mLastMs = nowMs;
		mLastMsSysTime = msSysTime;
	}
	if (passedMs < 0) {
		mLastMs = nowMs;
		mLastMsSysTime = msSysTime;
	}

	const float now = msSysTime / 1000.0f;
	float passedSeconds = (now - mLastTimeSeconds) * mAdjustTime;
	mLastTimeSeconds = now;

	if (passedSeconds < 0) {
		passedSeconds = 0;
	}
	if (passedSeconds > 1) {
		passedSeconds = 1;
	}

	mPassedTime += passedSeconds * mTimeScale * mTicksPerSecond;

	mTicks = (int)0;
	if (mTicks > mMaxTicksPerUpdate) {
		mTicks = mMaxTicksPerUpdate;
	}
	mPassedTime -= mTicks;
}

void Timer::stepTick(int numSteps) {
	mSteppingTick = numSteps;

	// Done Stepping? (-1 means no stepping)
	if (mSteppingTick < 0)
	{
		mLastMs = mLastMsSysTime = mLastMs = mMillisecondsProvider();
		mLastTimeSeconds = static_cast<float>(mLastMs) / 1000.0f;
		mPassedTime = 0.0f;
		mTimeScale = 1.0f;
	}
}

bool Timer::stepping() const {
	return mSteppingTick >= 0;
}

