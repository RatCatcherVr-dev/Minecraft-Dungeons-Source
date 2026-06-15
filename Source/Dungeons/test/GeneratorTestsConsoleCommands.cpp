#include "Dungeons.h"
#include "CommonTypes.h"
#include "GeneratorTest.h"
#include "game/Game.h"
#include "game/mission/MissionDefs.h"
#include "game/objective/ObjectiveFactory.h"
#include "game/objective/ObjectivesSystem.h"
#include "game/util/ConsoleCommandHelpers.h"
#include "game/util/DungeonsTravelUtil.h"
#include "lovika/world/level/levelgen/LevelGen.h"
#include "lovika/world/level/levelgen/generator/Generator.h"
#include "util/Algo.hpp"
#include "util/EnumUtil.h"
#include "util/StringUtil.h"
#include "util/SharedRandom.h"
#include "util/UELogHelpers.h"
#include "util/telemetry/AnalyticsDataTypes.h"
#include <numeric>
#include <map>

namespace generatortest {

void validateAndGenerateLevelEx(const LevelLoadConfig& config, const CanContinuePred& continuePred, size_t maxRuns, FOutputDevice& out) {
	const auto genData = loadAndValidateGeneratorData(config, out);
	if (!genData) {
		return;
	}
	const auto runs = generateLevelEx(genData->runner(), config.level.level, continuePred, maxRuns, 1);
	if (runs.failed) {
		out.Logf(ELogVerbosity::Error, TEXT("Can not generate level: %s . Failed %d times (out of %d)"), *config.level.filename, runs.failed, maxRuns);
	} else {
		out.Logf(ELogVerbosity::Display, TEXT("Generated level %s %d times without errors."), *config.level.filename, maxRuns);
	}
	out.Logf(ELogVerbosity::Display, TEXT("\n"));
}

TOptional<RandomSeed> findTileSeed(const levelgen::Runner& runner, ELevelNames level, const FString& tileId, int maxRuns = 50) {
	TOptional<RandomSeed> foundInSeed;

	auto containsTile = [&, id = stringutil::toStdString(tileId.ToLower())](const levelgen::Result& result) {
		if (algo::any_of(result.levelDef.tiles, RETLAMBDA(it.metaTile.lowerId == id || it.metaTile.tile().lowerId() == id))) {
			foundInSeed = result.finalSeed;
			return false;
		}
		return true;
	};
	const auto seedsPerRun = 20;
	const auto totalSeedsTried = maxRuns * seedsPerRun;
	const auto startSeed = 1 + RandomSeed(Math::roundTo(Util::sharedRandom().nextInt(99000), totalSeedsTried));
	generateLevelEx(runner, level, containsTile, maxRuns, startSeed, seedsPerRun);
	return foundInSeed;
}


void GenerateLevel(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	const auto level = ArgAsLevel(commands, 0);
	const auto maxRuns = ArgAsInt(commands, 1).Get(200);
	if (!level) {
		out.Logf(ELogVerbosity::Error, TEXT("A level name (e.g. pumpkinpastures) must be provided."));
		return;
	}
	validateAndGenerateLevelEx({ level.GetValue(), levelsettingsutil::getProgressionKeys(world) }, RetryAsLongAsPossible, maxRuns, out);
}

void GenerateAllLevels(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	const size_t maxRuns = ArgAsInt(commands, 0).Get(50);
	const auto unlockKeys = levelsettingsutil::getProgressionKeys(world);

	for (const auto* missiondef : missions::getAll()) {
		validateAndGenerateLevelEx({ LevelArg(missiondef->level()), unlockKeys }, RetryAsLongAsPossible, maxRuns, out);
	}
}

size_t calculateLevelHash(const LevelArg& level, int runs) {
	size_t hash = 0;
	validateAndGenerateLevelEx({ level, levelsettingsutil::getDefaultProgressionKeys() }, [&hash](const levelgen::Result& result) {
		hash = hash * 13 + hashCode(generator::tilesToPlaceResults(result.levelDef.tiles));
		return true;
	}, runs, outputs::None());

	return hash;
}

void HashLevel(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	const auto startTimeSeconds = FPlatformTime::Seconds();

	const auto level = ArgAsLevel(commands, 0);
	const auto maxRuns = ArgAsInt(commands, 1).Get(50);
	if (!level) {
		out.Logf(ELogVerbosity::Log, TEXT("usage: Test.Generator.HashLevel <level-name> [runs=50]"));
		return;
	}
	out.Logf(ELogVerbosity::Log, TEXT("%30s %lld"), *GetEnumValueToStringStripped(level->level), calculateLevelHash(level.GetValue(), maxRuns));
	out.Logf(ELogVerbosity::Log, TEXT("Generation duration: %f s."), FPlatformTime::Seconds() - startTimeSeconds);
}

void HashAllLevels(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	const auto runsPerLevel = ArgAsInt(commands, 0).Get(50);
	const auto startTimeSeconds = FPlatformTime::Seconds();

	size_t combinedHash = 0;

	for (const auto* missiondef : missions::getAll()) {
		if (!missiondef->isTrial()) {
			const auto hash = calculateLevelHash(missiondef->level(), runsPerLevel);
			const auto missionName = GetEnumValueToStringStripped(missiondef->level());
			out.Logf(ELogVerbosity::Log, TEXT("%30s %lld"), *missionName, hash);

			combinedHash = combinedHash * 13 + hash;
		}
	}
	out.Logf(ELogVerbosity::Log, TEXT("Combined hash (for %d runs per level) : %d"), runsPerLevel, combinedHash);
	out.Logf(ELogVerbosity::Log, TEXT("Generation duration: %f s."), FPlatformTime::Seconds() - startTimeSeconds);
}

void TestLevelObjectives(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	const auto level = ArgAsLevel(commands, 0);
	const auto maxRuns = ArgAsInt(commands, 1).Get(1);
	if (!level) {
		out.Logf(ELogVerbosity::Error, TEXT("A level name (e.g. pumpkinpastures) must be provided."));
		return;
	}

	const auto genData = loadAndValidateGeneratorData({ level.GetValue(), levelsettingsutil::getProgressionKeys(world) }, out);
	if (!genData) {
		return;
	}
	generateGame(genData->runner(), level->level, [](GameState state) {
		auto mObjectives = std::make_unique<game::objective::ObjectivesSystem>(
			state.game,
			game::objective::create(state.levelDef.levelDef.data.objectives),
			&state.log
		);
		mObjectives->start_DEBUG_DO_NOT_USE();
		return true;
	}, maxRuns, *world, out);
}

void TestAllLevelObjectives(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	for (auto index = enum_cast(ELevelNames::Invalid) + 1; index < enum_cast(ELevelNames::count); ++index) {
		const auto level = static_cast<ELevelNames>(index);

		TestLevelObjectives({ GetEnumValueToStringStripped(level) }, world, out);
	}
}

void GetUsedTilesInLevel(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	const auto level = ArgAsLevel(commands, 0);
	const auto maxRuns = ArgAsInt(commands, 1).Get(100);
	if (!level) {
		out.Logf(ELogVerbosity::Error, TEXT("A level name (e.g. pumpkinpastures) must be provided."));
		return;
	}

	const auto genData = loadAndValidateGeneratorData({ level.GetValue(), levelsettingsutil::getProgressionKeys(world) }, out);
	if (!genData) {
		return;
	}

	std::set<std::string> tileNames;

	generateLevelEx(
		genData->runner(),
		level->level,
		[&](const levelgen::Result& result) {
			for (auto& tile : result.levelDef.tiles) {
				tileNames.insert(tile.metaTile.tile().id());
			}
			return true;
		},
		maxRuns
	);

	const auto tileNamesString = std::accumulate(
		tileNames.begin(),
		tileNames.end(),
		std::string {},
		[](auto prev, auto current) { return prev + '\n' + current; }
	);

	const auto summary = "Found " + std::to_string(tileNames.size()) + " tiles.";

	out.Log(stringutil::toFString(tileNamesString + '\n' + summary));
}

void GetUsedSubdungeonsInLevel(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	const auto level = ArgAsLevel(commands, 0);
	const auto maxRuns = ArgAsInt(commands, 1).Get(100);
	if (!level) {
		out.Logf(ELogVerbosity::Error, TEXT("A level name (e.g. pumpkinpastures) must be provided."));
		return;
	}

	const auto genData = loadAndValidateGeneratorData({ level.GetValue(), levelsettingsutil::getProgressionKeys(world) }, out);
	if (!genData) {
		return;
	}
	std::map<FString, size_t> totalSubdungeonCounts;
	for (int i = 1; i < genData->sourceData.level.dungeons.size(); ++i) {
		// Add all sub-dungeons, so that the printout in the end shows the non-found as well
		totalSubdungeonCounts[stringutil::toFString(genData->sourceData.level.dungeons[i].id.id)] = 0;
	}

	generateLevelEx(
		genData->runner(),
		level->level,
		[&](const levelgen::Result& result) {
			const auto dungeonIndices = algo::map_as<std::set<size_t>>(result.levelDef.stretches, RETLAMBDA(it.def.dungeonIndex));
			for (auto dungeonIndex : dungeonIndices) {
				if (dungeonIndex > 0) { // 0 == main-path
					totalSubdungeonCounts[stringutil::toFString(result.levelDef.data.dungeons[dungeonIndex].id.id)]++;
				}
			}
			return true;
		},
		maxRuns
	);

	auto orderedSubdungeonCounts = algo::map_vector(totalSubdungeonCounts, RETLAMBDA(std::make_pair(it.second, it.first)));
	std::sort(orderedSubdungeonCounts.begin(), orderedSubdungeonCounts.end(), std::greater<Pair<size_t, FString>>{});

	const auto SubDungeonCountToString = [maxRuns](const Pair<size_t, FString>& it) {
		return FString::Printf(TEXT("    %s %d/%d (%.1f%%)"),
			*it.second,
			it.first,
			maxRuns,
			Math::roundTo(100.0f * it.first / maxRuns, 0.1f)
		);
	};

	TArray<FString> lines;
	lines.Add("Sub-dungeons:");
	lines.Append(algo::copy_if_map_tarray(orderedSubdungeonCounts, RETLAMBDA(it.first > 0), SubDungeonCountToString));
	lines.Add("-------------");
	lines.Append(algo::copy_if_map_tarray(orderedSubdungeonCounts, RETLAMBDA(it.first == 0), SubDungeonCountToString));

	out.Log(FString::Join(lines, TEXT("\n")));
}

std::map<BlockID, int> getUsedBlockCountsInLevel(const LevelArg& level, int maxRuns, FOutputDevice& out) {
	const auto genData = loadAndValidateGeneratorData({ level, levelsettingsutil::getDefaultProgressionKeys() }, out);
	if (!genData) {
		return {};
	}

	std::map<BlockID, int> blockCounts;
	generateLevelEx(
		genData->runner(),
		level.level,
		[&](const levelgen::Result& result) {
			for (auto& tile : result.levelDef.tiles) {
				auto& blocks = tile.placeResult.tilePlacement.tile().blocks();
				algo::for_each(BlockCuboid::fromSize(blocks.size()), RETLAMBDA(blockCounts[blocks.getBlockId(it)]++));
			}
			return true;
		},
		maxRuns
	);
	return blockCounts;
}

void printUsedBlockCounts(const std::map<BlockID, int>& blocks, FOutputDevice& out) {
	auto ids = algo::map_cast<std::vector<std::pair<BlockID, int>>>(blocks);
	std::sort(ids.begin(), ids.end());

	const auto blockCountString = std::accumulate(
		ids.begin(),
		ids.end(),
		std::string{},
		[](auto prev, auto current) { return prev + '\n' + std::to_string(current.first) + ": " + std::to_string(current.second); }
	);

	const auto summary = "Found " + std::to_string(blocks.size()) + " blocks.";

	out.Log(stringutil::toFString(blockCountString + '\n' + summary));
}

void GetUsedBlocksInLevel(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	const auto level = ArgAsLevel(commands, 0);
	const auto maxRuns = ArgAsInt(commands, 1).Get(100);
	if (!level) {
		out.Logf(ELogVerbosity::Error, TEXT("A level name (e.g. pumpkinpastures) must be provided."));
		return;
	}
	printUsedBlockCounts(getUsedBlockCountsInLevel(level.GetValue(), maxRuns, out), out);
}

void GetUsedBlocksInAllLevels(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	const auto maxRuns = ArgAsInt(commands, 0).Get(10);

	std::map<BlockID, int> blockCounts;

	for (const auto* missiondef : missions::getAll()) {
		for (const auto& kv : getUsedBlockCountsInLevel(missiondef->level(), maxRuns, out)) {
			blockCounts[kv.first] += kv.second;
		}
	}

	printUsedBlockCounts(blockCounts, out);
}

void ListLevels(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	TArray<FString> names;
	for (int i = enum_cast(ELevelNames::Invalid) + 1; i < enum_cast(ELevelNames::count); ++i) {
		names.Add(GetEnumValueToStringStripped(static_cast<ELevelNames>(i)));
	}
	const auto summary = "Listed " + FString::FromInt(names.Num()) + " levels.";
	out.Logf(TEXT("%s\n%s"), *FString::Join(names, TEXT("\n")), *summary);
}

void FindTileSeed(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	const auto level = ArgAsLevel(commands, 0);
	const auto tileId = ArgAsFString(commands, 1);

	if (!level || !tileId) {
		out.Logf(ELogVerbosity::Error, TEXT("A level name and a tile-id must be provided (e.g. pumpkinpastures pp_start)"));
		return;
	}

	LogRecorder logRecorder;
	const auto genData = loadAndValidateGeneratorData({ level.GetValue(), levelsettingsutil::getProgressionKeys(world) }, logRecorder);
	if (!genData) {
		playbackLog(logRecorder.getEntries(), out);
		return;
	}
	auto seed = findTileSeed(genData->runner(), level->level, tileId.GetValue(), ArgAsInt(commands, 2).Get(250));
	if (!seed) {
		out.Logf(ELogVerbosity::Warning, TEXT("Couldn't find tile id: %s"), *tileId.GetValue());
		return;
	}
	out.Logf(TEXT("Found tile in seed: %d"), seed.GetValue());
}

void StartTileLevel(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	const auto level = ArgAsLevel(commands, 0);
	const auto tileId = ArgAsFString(commands, 1);

	if (!level || !tileId) {
		out.Logf(ELogVerbosity::Error, TEXT("A level name and a tile-id must be provided (e.g. pumpkinpastures pp_start)"));
		return;
	}
	const auto genData = loadAndValidateGeneratorData({ level.GetValue(), levelsettingsutil::getProgressionKeys(world) }, out);
	if (!genData) {
		return;
	}
	auto seed = findTileSeed(genData->runner(), level->level, tileId.GetValue(), ArgAsInt(commands, 2).Get(250));
	if (!seed) {
		out.Logf(ELogVerbosity::Warning, TEXT("Couldn't find tile id: %s"), *tileId.GetValue());
		return;
	}
	out.Logf(TEXT("Found tile in seed: %d"), seed.GetValue());
	travelutil::ServerTravelToGameMap(world, levelsettingsutil::generateDevelopmentMissionSettings(
		world,
		level->level,
		level->filename,
		EGameDifficulty::Difficulty_1,
		EThreatLevel::Threat_1,
		{0},
		{0, 0},
		seed.GetValue()
	));
}

const FAutoConsoleCommand GenerateLevelCommand(TEXT("Test.Generator.GenerateLevel")
	, TEXT("Generates a given level a bunch of times")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&GenerateLevel)
	, ECVF_Cheat);

const FAutoConsoleCommand GenerateAllLevelsCommand(TEXT("Test.Generator.GenerateAllLevels")
	, TEXT("Generates all levels a bunch of times")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&GenerateAllLevels)
	, ECVF_Cheat);

const FAutoConsoleCommand HashAllLevelsCommand(TEXT("Test.Generator.HashAllLevels")
	, TEXT("Prints out combined level hashes after generating all levels a bunch of times")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&HashAllLevels)
	, ECVF_Cheat);

const FAutoConsoleCommand HashLevelCommand(TEXT("Test.Generator.HashLevel")
	, TEXT("Prints out combined level hash after generating the level a bunch of times")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&HashLevel)
	, ECVF_Cheat);

const FAutoConsoleCommand LevelObjectivesCommand(TEXT("Test.Objectives.Level")
	, TEXT("Test a level's objectives")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&TestLevelObjectives)
	, ECVF_Cheat);


const FAutoConsoleCommand AllLevelObjectivesCommand(TEXT("Test.Objectives.AllLevels")
	, TEXT("Test all levels' objectives")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&TestAllLevelObjectives)
	, ECVF_Cheat);


const FAutoConsoleCommand GetUsedTilesCommand(TEXT("Test.Level.GetUsedTiles")
	, TEXT("Get a list of used tiles for a level")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&GetUsedTilesInLevel)
	, ECVF_Cheat);

const FAutoConsoleCommand GetUsedSubdungeonsCommand(TEXT("Test.Level.GetUsedSubdungeons")
	, TEXT("List a list of used/found subdungeons for a level")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&GetUsedSubdungeonsInLevel)
	, ECVF_Cheat);

const FAutoConsoleCommand GetUsedBlocksCommand(TEXT("Test.Level.GetUsedBlocks")
	, TEXT("Get a list of used blocks for a level. Arguments: levelname [runs=100]")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&GetUsedBlocksInLevel)
	, ECVF_Cheat);

const FAutoConsoleCommand GetUsedBlocksInAllLevelsCommand(TEXT("Test.Level.GetUsedBlocksInAllLevels")
	, TEXT("Get a list of used blocks for all levels. Arguments: [runs=10]")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&GetUsedBlocksInAllLevels)
	, ECVF_Cheat);

const FAutoConsoleCommand LevelListCommand(TEXT("Test.Level.List")
	, TEXT("Print list of all levels")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&ListLevels)
	, ECVF_Cheat);

const FAutoConsoleCommand FindTileSeedCommand(TEXT("Test.Generator.FindTileSeed")
	, TEXT("Find seed where a given tile is used")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&FindTileSeed)
	, ECVF_Cheat);

const FAutoConsoleCommand StartTileLevelCommand(TEXT("Test.Generator.StartTileLevel")
	, TEXT("Start a level with a seed where a given tile is used")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&StartTileLevel)
	, ECVF_Cheat);

}
