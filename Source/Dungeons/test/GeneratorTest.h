#pragma once

#include "CommonTypes.h"
#include "lovika/world/level/levelgen/LevelGen.h"
#include "lovika/world/level/levelgen/LevelGenUtil.h"
#include "game/util/ConsoleCommandHelpers.h"

class FOutputDevice;

namespace generatortest {

using CanContinuePred = Pred<const levelgen::Result&>;
const CanContinuePred RetryAsLongAsPossible = [](auto&) { return true; };

struct GameState {
	game::Game& game;
	game::LevelDef& levelDef;
	const levelgen::Result& result;
	FOutputDevice& log;
};
using GameCanContinuePred = Pred<GameState>;

struct LevelLoadConfig {
	LevelLoadConfig(LevelArg, TArray<FString> unlockKeys, RandomSeed = 0);

	LevelArg level;
	TArray<FString> unlockKeys;
	RandomSeed seed;
};

struct Runs {
	size_t successful = 0;
	size_t failed = 0;
};

namespace outputs {

FOutputDeviceNull& None();

}

FLevelSettings createLevelSettings(const LevelLoadConfig&);
TOptional<levelgen::LevelGenData> loadAndValidateGeneratorData(const LevelLoadConfig&, FOutputDevice&);

Runs generateLevelEx(const levelgen::Runner&, ELevelNames, const CanContinuePred&, int maxRuns, RandomSeed = 1, int maxTries = 100);
Runs generateGame(const levelgen::Runner&, ELevelNames, const GameCanContinuePred&, int maxRuns, UWorld&, FOutputDevice&, RandomSeed = 1);

}
