#include "Dungeons.h"
#include "Difficulty.h"
#include "game/mission/MissionDefs.h"
#include "util/FloatRange.h"
#include "ThreatLevel.h"
#include "game/LevelSettings.h"
#include "game/item/power/ItemPowerStats.h"
#include "util/Algo.hpp"
#include <AssertionMacros.h>

namespace game {

namespace combineddifficulty {

/*
Currently intended combined difficulty at the start of each difficulty.
Default:
	1.0
Adventure:
	1.15
Apocalypse:
	1.35
Presumptive-non-existent-difficulty-4 (which we lerp towards at end of apocalypse):
	1.60
*/
static FloatRange getMultiplierRange(EGameDifficulty difficulty) {
	switch (difficulty) {
	case EGameDifficulty::Difficulty_1:
		return FloatRange(1.0f, 1.15f);
	case EGameDifficulty::Difficulty_2:
		return FloatRange(1.15f, 1.35f);
	case EGameDifficulty::Difficulty_3:
		return FloatRange(1.35f, 1.6f);
	default:
		ensure(false && "whops, this is not a valid difficulty");
		return FloatRange(1.0f, 1.0f);
	}
}

//Towards the end of each difficulty, the later threat levels start to sneak up towards the next difficulty.
static const TMap<EThreatLevel, float> ThreatLevelRangeLerp = {
	{ EThreatLevel::Threat_1, 0.0f },
	{ EThreatLevel::Threat_2, 0.0f },
	{ EThreatLevel::Threat_3, 0.0f },
	{ EThreatLevel::Threat_4, 0.0f },
	{ EThreatLevel::Threat_5, 0.33f },
	{ EThreatLevel::Threat_6, 0.66f },
	{ EThreatLevel::Threat_7, 1.0f },
};

float calculateCombinedDifficulty(const FDifficulty& difficulty) {
	static constexpr float extraChallengeFractionExponent = 2.0f;

	const float globalProgressFraction = difficulty.globalThreatFraction() +
		difficulty.extraChallengeAsGlobalProgressFraction(extraChallengeFractionExponent) +
		difficulty.endlessStruggleAsGlobalProgressFraction() + difficulty.emergentDifficultyFraction();
	const float itemEfficiency = game::item::power::GetExponentialEfficiencyFromProgressFraction(globalProgressFraction);
	const float baseScale = itemEfficiency - 0.25f; // Difficulty starts lower than item efficiency

	const FloatRange multiplierRange = getMultiplierRange(difficulty.chosen());
	const float lerpAlpha = ThreatLevelRangeLerp.Contains(difficulty.threatLevel()) ? ThreatLevelRangeLerp[difficulty.threatLevel()] : 0.0f;
	const float multiplier = multiplierRange.clampedLerp(lerpAlpha);

	return baseScale * multiplier - 1;
}

}

FDifficulty::FDifficulty( EGameDifficulty chosen, EThreatLevel threatLevel, EExtraChallenge extraChallenge, FEndlessStruggle endlessStruggle, FEmergentDifficulty emergentDifficulty)
	: mChosen(chosen)
	, mThreatLevel(threatLevel)
	, mExtraChallenge(extraChallenge)
	, mEndlessStruggle(endlessStruggle)
	, mEmergentDifficulty(emergentDifficulty)
{

	mMission = threatquery::toMissionDifficultyFraction(threatLevel);
	mExtra = extrachallengequery::toExtraFraction(extraChallenge);

	mCombined = combineddifficulty::calculateCombinedDifficulty(*this);
}

FDifficulty::FDifficulty(EGameDifficulty chosen, float missionDifficultyFraction)
	: FDifficulty(chosen, threatquery::toClosestThreatFromFraction(missionDifficultyFraction)) {
}

float FDifficulty::endlessStruggleFraction() const {
	return mEndlessStruggle.Value / static_cast<float>(NumberOfEndlessStruggleLevels);
}

float FDifficulty::emergentDifficultyFraction() const
{
	return emergentDifficulty().raidDifficulty * emergentDifficulty().perGlobalThreatFraction / numGlobalThreats();
}

float FDifficulty::globalThreatFractionClampedBetweenDifficulties(FDifficulty lower, FDifficulty higher) const {
	const FloatRange threatFractionRange(
		lower.globalThreatFraction(),
		higher.globalThreatFraction()
	);
	return threatFractionRange.clampedFractionAt(globalThreatFraction());
}

int FDifficulty::numGlobalThreats() {
	return NumberOfDifficulties * NumberOfThreatLevels - (NumberOfDifficulties-1) * NumberOfOverlappingThreatLevels;
}


int FDifficulty::globalThreatIndex() const {
	return (enum_cast(mChosen) - EGameDifficultyFirstIndex) * NumberOfThreatLevels + (enum_cast(mThreatLevel) - EThreatLevelFirstIndex - (enum_cast(mChosen)-1) * NumberOfOverlappingThreatLevels);
}

int FDifficulty::extraChallengeIndex() const {
	return enum_cast(mExtraChallenge) - EExtraChallengeFirstIndex;
}

float FDifficulty::globalThreatFraction() const {
	return globalThreatIndex() / static_cast<float>(numGlobalThreats() - 1);
}

//Extra challenge 1/2/3 should reflect the difficulty increase of 0.33/1.333/3 threat levels.
float FDifficulty::extraChallengeAsGlobalProgressFraction(const float extraChallengeFractionExponent) const {
	static constexpr float EachExtraChallengeBaseThreatLevels = 1.f;
	const float extraChallengeFractionPowed = FMath::Pow(extraChallengeFraction(), extraChallengeFractionExponent);
	return extraChallengeFractionPowed * (float)NumberOfExtraChallenges * EachExtraChallengeBaseThreatLevels / (float)numGlobalThreats();
}

float FDifficulty::endlessStruggleAsGlobalProgressFraction() const {
 	static constexpr float EachEndlessStruggleApproximateThreatLevels = 1.0f;
	return endlessStruggleFraction() * (float)NumberOfEndlessStruggleLevels * EachEndlessStruggleApproximateThreatLevels / (float)numGlobalThreats();
}

float FDifficulty::combinedFraction() const {
	return globalThreatFraction();
}

float FDifficulty::combinedFractionIncludingEndlessStruggle() const {
	return globalThreatFraction() + endlessStruggleAsGlobalProgressFraction();
}

bool FDifficulty::anyEndlessStruggle() const {
	return endlessStruggle().Value > 0;
}

/*static*/ const FDifficulty FDifficulty::DEFAULT { difficultyquery::Default, threatquery::Default, extrachallengequery::Default };
/*static*/ const FDifficulty FDifficulty::LOWEST  { difficultyquery::First, threatquery::First, extrachallengequery::First };
/*static*/ const FDifficulty FDifficulty::HIGHEST { difficultyquery::Last, threatquery::Last, extrachallengequery::Last };

}

namespace difficultyquery {

const std::vector<EGameDifficulty> AllDifficulties = [] {
	std::vector<EGameDifficulty> out;
	for (int i = enum_cast(First), max = enum_cast(Last); i <= max; ++i) {
		out.push_back(static_cast<EGameDifficulty>(i));
	}
	return out;
}();

std::vector<EGameDifficulty> lower(EGameDifficulty difficulty, bool inclusive) {
	return algo::copy_if(AllDifficulties, RETLAMBDA(inclusive ? it <= difficulty : it < difficulty));
}

std::vector<EGameDifficulty> higher(EGameDifficulty difficulty, bool inclusive) {
	return algo::copy_if(AllDifficulties, RETLAMBDA(inclusive ? it >= difficulty : it > difficulty));
}

std::vector<EGameDifficulty> inclusiveBetween(EGameDifficulty low, EGameDifficulty high) {
	return algo::copy_if(AllDifficulties, RETLAMBDA(it >= low && it <= high));
}

bool isUnlockedByDefault(const EGameDifficulty Difficulty) {
	return UnlockedByDefault >= Difficulty;
}

int getLivesForDifficulty(const EGameDifficulty Difficulty)
{
	switch (Difficulty) {
	case EGameDifficulty::Difficulty_3:
		return 3;
	case EGameDifficulty::Difficulty_2:
		return 3;
	case EGameDifficulty::Difficulty_1:
		return 3;
	default:
		break;
	}
	return 5;
}

TOptional<game::FDifficulty> getPreviousDifficulty(const game::FDifficulty& Difficulty) {
	if (Difficulty.extraChallenge() > extrachallengequery::First) {
		return game::FDifficulty(Difficulty.chosen(), Difficulty.threatLevel(), static_cast<EExtraChallenge>(enum_cast(Difficulty.extraChallenge()) - 1), Difficulty.endlessStruggle());
	} else if( Difficulty.endlessStruggle().Value > 0 ) {
		return game::FDifficulty(Difficulty.chosen(), Difficulty.threatLevel(), Difficulty.extraChallenge(), FEndlessStruggle{Difficulty.endlessStruggle().Value-1} );
	} else if (Difficulty.threatLevel() > threatquery::First) {
		return game::FDifficulty(Difficulty.chosen(), static_cast<EThreatLevel>(enum_cast(Difficulty.threatLevel()) - 1), Difficulty.extraChallenge());
	} else if (Difficulty.chosen() > difficultyquery::First) {
		return game::FDifficulty(static_cast<EGameDifficulty>(enum_cast(Difficulty.chosen()) - 1), threatquery::Last, Difficulty.extraChallenge());
	}
	return {};
}

TOptional<EGameDifficulty> getPreviousDifficulty(EGameDifficulty Difficulty) {
	if (Difficulty > difficultyquery::First) {
		return static_cast<EGameDifficulty>(enum_cast(Difficulty) - 1);
	}
	return {};
}

TOptional<EGameDifficulty> getNextDifficulty(EGameDifficulty Difficulty) {
	if (Difficulty < difficultyquery::Last) {
		return static_cast<EGameDifficulty>(enum_cast(Difficulty) + 1);
	}
	return {};
}

#define LOCTEXT_NAMESPACE "Difficulty"	
FText getDifficultyDisplayName(EGameDifficulty Difficulty)
{
	switch (Difficulty) {
	case EGameDifficulty::Difficulty_3:
		return LOCTEXT("Difficulty_3", "Apocalypse");
	case EGameDifficulty::Difficulty_2:
		return LOCTEXT("Difficulty_2", "Adventure");
	case EGameDifficulty::Difficulty_1:
		return LOCTEXT("Difficulty_1", "Default");
	default:
		return FText::GetEmpty();
	}
}

int getNumThreatLevels(EGameDifficulty Difficulty) {
	return NumberOfThreatLevels;
}

int getNumEndlessStruggles(EGameDifficulty Difficulty) {
	switch (Difficulty) {
	default:
	case EGameDifficulty::ENUM_COUNT:
	case EGameDifficulty::Invalid:
	case EGameDifficulty::Difficulty_1:
	case EGameDifficulty::Difficulty_2:
		return 0;
	case EGameDifficulty::Difficulty_3:
		return NumberOfEndlessStruggleLevels;
	}
}

#undef LOCTEXT_NAMESPACE

}
