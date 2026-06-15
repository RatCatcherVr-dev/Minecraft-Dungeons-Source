#pragma once

#include <array>
#include <chrono>
#include "GenericPlatform/GenericPlatformMath.h"
#include <DelegateCombinations.h>

constexpr auto DELTA_ARRAY_SIZE = 103;
constexpr auto FRAME_COUNT_ARRAY_SIZE = 122;
constexpr auto STALL_THRESHOLD_MS = std::chrono::milliseconds(-10);
constexpr auto STALL_EVENT_TIME = std::chrono::seconds(20);

class FrameTimeTracker {
public:
	FrameTimeTracker() {
		Reset();
	}

	void Update(float deltaTime) {
		if (deltaTime <= 0.0) {
			return;
		}

		auto deltaTimeMs = std::chrono::milliseconds(FMath::Min(FMath::FloorToInt(deltaTime * 1000), FRAME_COUNT_ARRAY_SIZE - 1));

		// Maintain a count of the framerates
		++mFrameCounts[deltaTimeMs.count()];
		
		if (mLastFrameDelta.count() > 0) {
			auto deltaMS = deltaTimeMs - mLastFrameDelta;

			auto arrayIndex = Math::clamp(static_cast<int>(51 + Math::floor(deltaMS.count() / 20.0f)), 0, 102);
			++mFrameDeltaCounts[arrayIndex];
		}
		mLastFrameDelta = deltaTimeMs;
	}

	void Reset() {
		mFrameCounts.fill(0);
		mFrameDeltaCounts.fill(0);
	}

	const std::array<uint32, FRAME_COUNT_ARRAY_SIZE>& GetFrameTimeCounts() const { return mFrameCounts; }
	const std::array<uint32, DELTA_ARRAY_SIZE>& GetFrameDeltaCounts() const { return mFrameDeltaCounts; }
private:
	std::array<uint32, FRAME_COUNT_ARRAY_SIZE> mFrameCounts;
	std::array<uint32, DELTA_ARRAY_SIZE> mFrameDeltaCounts;
	std::chrono::milliseconds mLastFrameDelta{ 0 };
};
