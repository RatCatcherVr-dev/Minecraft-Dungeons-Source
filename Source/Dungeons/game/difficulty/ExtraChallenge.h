#pragma once

#include "common_header.h"
#include "Internationalization/Text.h"
#include "ThreatLevel.h"

UENUM(BlueprintType)
enum class EExtraChallenge : uint8 {
	Invalid,
	NoExtraChallenge,
	Challenge_1,
	Challenge_2,
	Challenge_3,
	ENUM_COUNT,
};
ENUM_NAME(EExtraChallenge);

const size_t EExtraChallengeFirstIndex = (size_t)EExtraChallenge::NoExtraChallenge;
constexpr size_t NumberOfExtraChallenges{ (size_t)(EExtraChallenge::ENUM_COUNT) - EExtraChallengeFirstIndex - 1};

namespace extrachallengequery {
	const EExtraChallenge Default = EExtraChallenge::NoExtraChallenge;
	const EExtraChallenge First = EExtraChallenge::NoExtraChallenge;
	const EExtraChallenge Last = EExtraChallenge::Challenge_3;
	float toExtraFraction(EExtraChallenge);	
	FText getExtraChallengeDisplayName(EExtraChallenge);
	EExtraChallenge getExtraChallengeFromAmount(int amount);
	TOptional<EExtraChallenge> getNextExtraChallenge(EExtraChallenge);
};