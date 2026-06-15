#include "Dungeons.h"
#include "DifficultyRecommendation.h"
#include "game/mission/MissionDefs.h"
#include "DifficultyUtil.h"


namespace game {

const game::DifficultyStats DifficultyRecommendation::lowestStats = game::DifficultyStats{ game::FDifficulty::LOWEST };

DifficultyRecommendation::DifficultyRecommendation() {}
DifficultyRecommendation::DifficultyRecommendation(int displayItemPower, TOptional<FEmergentDifficulty> emergentDifficulty) {
	for (int c = enum_cast(extrachallengequery::First); c <= enum_cast(extrachallengequery::Last); c++) {
		const auto extraChallenge = static_cast<EExtraChallenge>(c);
		auto&& difficultyRecommendation = UMissionDefs::GetGearPowerDifficultyRecommendation(extraChallenge, displayItemPower);
		mRecommendedStats.Add(extraChallenge, game::DifficultyStats{ UDifficultyUtil::GetDifficultyWithEmergentDifficulty( difficultyRecommendation.CreateDifficulty(), emergentDifficulty) });
	}
}

const game::DifficultyStats& DifficultyRecommendation::getDifficultyStats(EExtraChallenge extraChallenge) const {
	if (auto statsptr = mRecommendedStats.Find(extraChallenge)) {
		return *statsptr;
	}
	return lowestStats;
}

const game::FDifficulty& DifficultyRecommendation::getDifficulty(EExtraChallenge extraChallenge) const {	
	return getDifficultyStats(extraChallenge).GetDifficulty();	
}

}
