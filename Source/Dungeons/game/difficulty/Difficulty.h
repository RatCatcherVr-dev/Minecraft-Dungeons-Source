#pragma once

#include "ThreatLevel.h"
#include "ExtraChallenge.h"
#include "EndlessStruggle.h"
#include "EmergentDifficulty.h"

UENUM(BlueprintType)
enum class EGameDifficulty : uint8 {
	Invalid,
	Difficulty_1, //default
	Difficulty_2, //adventure
	Difficulty_3, //apocalypse
	//Difficulty_4, //removed
	//Difficulty_5, //removed
	ENUM_COUNT,
};
ENUM_NAME(EGameDifficulty);

const size_t EGameDifficultyFirstIndex = (size_t)EGameDifficulty::Difficulty_1;
constexpr size_t NumberOfDifficulties{ (size_t)(EGameDifficulty::ENUM_COUNT) - EGameDifficultyFirstIndex };


namespace game {

struct FDifficulty {
	FDifficulty(EGameDifficulty chosen = EGameDifficulty::Difficulty_1, EThreatLevel threatLevel = EThreatLevel::Threat_1, EExtraChallenge extraChallenge = EExtraChallenge::NoExtraChallenge, FEndlessStruggle endlessStruggle = {0}, FEmergentDifficulty emregentDifficulty = {0, 0});
	FDifficulty(EGameDifficulty, float missionDifficultyFraction);

	EGameDifficulty chosen() const { return mChosen; }

	EThreatLevel threatLevel() const { return mThreatLevel; }
	float missionDifficultyFraction() const { return mMission; }
	FEndlessStruggle endlessStruggle() const { return mEndlessStruggle; }
	FEmergentDifficulty emergentDifficulty() const { return mEmergentDifficulty; }
	float endlessStruggleFraction() const;
	float emergentDifficultyFraction() const;

	float extraChallengeFraction() const { return mExtra; };
	EExtraChallenge extraChallenge() const { return mExtraChallenge; }

	float globalLinearScaleNormalized() const { return globalThreatFraction(); }
	float threatFraction() const { return mMission; }

	float combined() const { return mCombined; }
	float combinedFraction() const;

	float combinedFractionIncludingEndlessStruggle() const;
	bool anyEndlessStruggle() const;

	static int numGlobalThreats();
	int globalThreatIndex() const;
	int extraChallengeIndex() const;
	float globalThreatFraction() const;
	float globalThreatFractionClampedBetweenDifficulties(FDifficulty lower, FDifficulty higher) const;
	float extraChallengeAsGlobalProgressFraction(const float extraChallengeFractionExponent) const;
	float endlessStruggleAsGlobalProgressFraction() const;

	static const FDifficulty LOWEST;
	static const FDifficulty HIGHEST;
	static const FDifficulty DEFAULT;
private:
	EGameDifficulty mChosen;

	EThreatLevel mThreatLevel;
	EExtraChallenge mExtraChallenge;
	FEndlessStruggle mEndlessStruggle;
	FEmergentDifficulty mEmergentDifficulty;

	float mMission;
	float mExtra;

	float mCombined;
};

FORCEINLINE bool operator<(const FDifficulty& left, const FDifficulty& right) {
	return left.combinedFraction() < right.combinedFraction();
}

FORCEINLINE bool operator>(const FDifficulty& left, const FDifficulty& right) {
	return left.combinedFraction() > right.combinedFraction();
}

FORCEINLINE bool operator>=(const FDifficulty& left, const FDifficulty& right) {
	return left.combinedFraction() >= right.combinedFraction();
}

}

namespace difficultyquery {
	const EGameDifficulty First = EGameDifficulty::Difficulty_1;
	const EGameDifficulty Last  = EGameDifficulty::Difficulty_3;
	const EGameDifficulty UnlockedByDefault = EGameDifficulty::Difficulty_1;	
	const EGameDifficulty Default = EGameDifficulty::Difficulty_1;
	const EGameDifficulty RequiredForEndlessStuggle = EGameDifficulty::Difficulty_3;
	extern const std::vector<EGameDifficulty> AllDifficulties;

	std::vector<EGameDifficulty> lower(EGameDifficulty, bool inclusive);
	std::vector<EGameDifficulty> higher(EGameDifficulty, bool inclusive);
	std::vector<EGameDifficulty> inclusiveBetween(EGameDifficulty low, EGameDifficulty high);

	bool isUnlockedByDefault(EGameDifficulty);
	int getLivesForDifficulty(EGameDifficulty);
	TOptional<game::FDifficulty> getPreviousDifficulty(const game::FDifficulty&);
	TOptional<EGameDifficulty> getPreviousDifficulty(EGameDifficulty);
	TOptional<EGameDifficulty> getNextDifficulty(EGameDifficulty);

	FText getDifficultyDisplayName(EGameDifficulty);
	
	int getNumThreatLevels(EGameDifficulty Difficulty);	
	int getNumEndlessStruggles(EGameDifficulty Difficulty);
};
