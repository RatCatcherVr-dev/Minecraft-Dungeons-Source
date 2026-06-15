#include "Dungeons.h"
#include "GeneratorTest.h"
#include "game/mission/MissionDefs.h"
#include "game/mission/state/MissionState.h"
#include "lovika/world/level/ChunkBlockSource.h"
#include "lovika/world/level/postprocess/PostProcess.h"
#include "lovika/world/level/postprocess/PostProcessConfigs.h"
#include "game/Game.h"

namespace generatortest {

LevelLoadConfig::LevelLoadConfig(LevelArg level, TArray<FString> unlockKeys, RandomSeed seed)
	: level(level)
	, unlockKeys(std::move(unlockKeys))
	, seed(seed) {
}

FLevelSettings createLevelSettings(const LevelLoadConfig& config) {
	const auto& difficulty = game::FDifficulty::DEFAULT;
	return FLevelSettings{
		FMissionState{
			FMissionDifficulty{
				config.level.level,
				difficulty.chosen(),
				difficulty.threatLevel(),
				difficulty.endlessStruggle(),
			},
			config.seed,
		},
		{0, 0},
		FLevelSettings::UNREAL_INGAME_MAP_NAME,
		config.level.filename,
		config.unlockKeys
	};
}

TOptional<levelgen::LevelGenData> loadAndValidateGeneratorData(const LevelLoadConfig& config, FOutputDevice& out) {
	return levelgen::createLevelGenData(createLevelSettings(config), &out);
}

Runs generateLevelEx(const levelgen::Runner& runner, ELevelNames level, const CanContinuePred& continuePred, int maxRuns, RandomSeed startSeed, int maxTries) {
	const auto& metaScorer = missions::get(level).metaScorer();
	Runs out;

	for (int i = 0; i < maxRuns; ++i) {
		auto result = runner.run(startSeed + i * maxTries, metaScorer, maxTries);
		if (result.success) {
			out.successful++;
			if (!continuePred(result)) { break; }
		}
		else {
			out.failed++;
		}
	}
	return out;
}

Runs generateGame(const levelgen::Runner& runner, ELevelNames level, const GameCanContinuePred& gameContinuePred, int maxRuns, UWorld& world, FOutputDevice& out, RandomSeed startSeed) {
	const auto levelCallback = [&](const levelgen::Result& result) {
		game::LevelDef levelDef{ result.levelDef, result.finalSeed };

		auto placeResults = tilesToPlaceResults(result.levelDef.tiles);
		auto region = createChunkBlockSourceFromTilePlacements(PlacedTiles(placeResults).placements()); //@placed @lazy	
		auto processResult = postprocess::run(*region, result.levelDef, postprocess::configs::OnlyDoors());
		levelDef.tileAreas = processResult.tileAreas;
		for (auto& placeResult : placeResults) {
			const auto& tp = placeResult.tilePlacement;
			placeResult.tilePlacement = TilePlacement(tp.metaIdHACK(), tp.tile(), tp.placement(), tp);
		}
		levelDef.placedTiles = PlacedTiles(placeResults);

		const FMissionState missionState {
			FMissionDifficulty{level, difficultyquery::Default, threatquery::Default, {}},
			result.finalSeed
		};
		game::Game game(world, levelDef, missionState, false);
		return gameContinuePred({ game, levelDef, result, out });
	};
	return generateLevelEx(runner, level, levelCallback, maxRuns, startSeed);
}

FOutputDeviceNull& outputs::None() {
	static FOutputDeviceNull nullOutput;
	return nullOutput;
}

}
