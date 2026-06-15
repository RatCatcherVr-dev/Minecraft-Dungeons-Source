#pragma once

#include "CommonTypes.h"
#include "game/Difficulty/DifficultyStats.h"
#include "game/levels.h"
#include <Set.h>

namespace game {

struct Settings {
	FDifficulty difficulty;	
	DifficultyStats difficultyStats;
	RandomSeed randomSeed;
	ELevelNames levelName;

	Settings()
		: Settings(FDifficulty::DEFAULT, 0, ELevelNames::Invalid) {
	}
	
	Settings(FDifficulty difficulty, RandomSeed randomSeed, ELevelNames levelName)
		: difficulty(difficulty)
		, difficultyStats{ difficulty }
		, randomSeed(randomSeed)
		, levelName(levelName) {
	}
};

}
