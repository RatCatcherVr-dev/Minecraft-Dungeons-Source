#pragma once

#include "common_header.h"
#include <vector>

UENUM(BlueprintType)
enum class EThreatLevel : uint8 {
	Invalid,
	Threat_1,
	Threat_2,
	Threat_3,
	Threat_4,
	Threat_5,
	Threat_6,
	Threat_7,
	ENUM_COUNT,
};
ENUM_NAME(EThreatLevel);

const size_t EThreatLevelFirstIndex = (size_t)EThreatLevel::Threat_1;
constexpr size_t NumberOfThreatLevels{ (size_t)(EThreatLevel::ENUM_COUNT) - EThreatLevelFirstIndex };
const size_t NumberOfOverlappingThreatLevels = 1;

namespace threatquery {
	const EThreatLevel First = EThreatLevel::Threat_1;
	const EThreatLevel Last = EThreatLevel::Threat_7;
	const EThreatLevel Default = EThreatLevel::Threat_1;
	const EThreatLevel UnlockedByDefault = EThreatLevel::Threat_3;
	const EThreatLevel RequiredForEndlessStuggle = EThreatLevel::Threat_7;
	const int NumThreatLevelsToUnlockAboveCompleted = 2;
	extern const std::vector<EThreatLevel> AllThreatLevels;

	bool isUnlockedByDefault(EThreatLevel ThreatLevel);	
	float toMissionDifficultyFraction(EThreatLevel threatLevel);
	EThreatLevel toClosestThreatFromFraction(float fraction);
	EThreatLevel getUnlockedForCompleted(EThreatLevel threatLevel);
	EThreatLevel getCompletedForUnlocked(EThreatLevel threatLevel);
	TOptional<EThreatLevel> getNextThreatLevel(EThreatLevel);
	TOptional<EThreatLevel> getPreviousThreatLevel(EThreatLevel);
};