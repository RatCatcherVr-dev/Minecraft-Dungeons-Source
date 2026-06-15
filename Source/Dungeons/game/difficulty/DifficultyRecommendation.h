#pragma once

#include "Difficulty.h"
#include "DifficultyStats.h"
#include "ExtraChallenge.h"
#include "game/levels.h"

namespace game {

	struct DifficultyRecommendation {

		DifficultyRecommendation();
		DifficultyRecommendation(int displayItemPower, TOptional<FEmergentDifficulty> emergentDifficulty);

		const game::DifficultyStats& getDifficultyStats(EExtraChallenge) const;
		const game::FDifficulty& getDifficulty(EExtraChallenge) const;
	private:
		TMap<EExtraChallenge, game::DifficultyStats> mRecommendedStats;		

		static const game::DifficultyStats lowestStats;
	};
}
