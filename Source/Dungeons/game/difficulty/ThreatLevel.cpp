#include "Dungeons.h"
#include "ThreatLevel.h"
#include "util/FloatRange.h"

namespace threatquery {

	bool isUnlockedByDefault(const EThreatLevel threatLevel) {
		return UnlockedByDefault >= threatLevel;
	}

	//First threat level should return 0.0f, last should return 1.0f;
	float toMissionDifficultyFraction(EThreatLevel threatLevel) {
		if (threatLevel == EThreatLevel::Invalid) {
			return 0.0f;
		}

		const int threatLevelIndex = enum_cast(threatLevel);
		const float threatLevelFraction = ((float)threatLevelIndex - (float)EThreatLevelFirstIndex) / (float)(NumberOfThreatLevels - 1);
		return FMath::Clamp(threatLevelFraction, 0.0f, 1.0f);
	}

	EThreatLevel toClosestThreatFromFraction(float fraction) {
		int index = FMath::RoundToInt(fraction * (NumberOfThreatLevels - 1));
		if (index < 0) {
			return threatquery::First;
		}
		else if (index >= NumberOfThreatLevels) {
			return threatquery::Last;
		}
		else {
			return static_cast<EThreatLevel>(index + enum_cast(threatquery::First));
		}
	}

	EThreatLevel getUnlockedForCompleted(EThreatLevel threatLevel)
	{
		const int threatLevelIndex = enum_cast(threatLevel);		
		return static_cast<EThreatLevel>( Math::min( (int)enum_cast(threatquery::Last), threatLevelIndex + NumThreatLevelsToUnlockAboveCompleted ) );
	}

	EThreatLevel getCompletedForUnlocked(EThreatLevel threatLevel)
	{
		const int threatLevelIndex = enum_cast(threatLevel);
		return static_cast<EThreatLevel>(Math::max((int)enum_cast(threatquery::First), threatLevelIndex - NumThreatLevelsToUnlockAboveCompleted));
	}

	TOptional<EThreatLevel> getNextThreatLevel(EThreatLevel threatLevel) {
		if (threatLevel < threatquery::Last) {
			return static_cast<EThreatLevel>(enum_cast(threatLevel) + 1);
		}
		return {};
	}

	TOptional<EThreatLevel> getPreviousThreatLevel(EThreatLevel threatLevel) {
		if (threatLevel > threatquery::First) {
			return static_cast<EThreatLevel>(enum_cast(threatLevel) - 1);
		}
		return {};
	}

}
