#include "Dungeons.h"
#include "game/Game.h"
#include "game/mission/hyper/HyperMissions.h"
#include "game/mission/hyper/HyperMissionUtil.h"
#include "game/objective/ObjectiveFactory.h"
#include "game/objective/ObjectivesSystem.h"
#include "game/util/ActorQuery.h"
#include "game/util/ConsoleCommandHelpers.h"
#include "game/util/ValueFormat.h"
#include "lovika/io/IoHyperTypes.h"
#include "lovika/world/level/levelgen/SourceData.h"
#include "lovika/world/level/levelgen/hyper/HyperStitcher.h"
#include "test/GeneratorTest.h"
#include "test/GeneratorTestUtil.h"
#include "util/Algo.h"
#include "util/CollectionUtils.h"
#include "util/RandomUtil.h"
#include "util/StringUtil.h"
#include <OutputDevice.h>
#include <unordered_set>


namespace hypercommands {

using namespace generatortest;

struct HyperMissionData
{
	enum class AncientDungeonType {
		None,
		Gold,
		Boss
	};
	AncientDungeonType getAncientDungeonType(const CaseInsensitiveId& dungeonId) const {
		if (const auto index = algo::index_of_if(hyperDungeons, RETLAMBDA(it.id == dungeonId))) {
			return hyperDungeons[index.GetValue()].isGoldDungeon ? AncientDungeonType::Gold : AncientDungeonType::Boss;
		}
		return AncientDungeonType::None;
	}

	TArray<FString> levelIds;
	std::vector<io::HyperDungeon> hyperDungeons;
};

TOptional<HyperMissionData> getHyperMissionData(FString levelFilename = "netherhypermission") {
	if (const auto hyperLevel = levelgen::sourcedata::hyperLevelFromFileSystem(levelFilename)) {
		return HyperMissionData {
			algo::map_tarray(hyperLevel->levelIds, RETLAMBDA(stringutil::toFString(it.id))),
			hyperLevel->hyperDungeons
		};
	}
	return {};
}

struct DungeonCounts {
	TMap<FString, int> boss;
	TMap<FString, int> gold;

	int totalBossCount() const { return algo::sum(boss, RETLAMBDA(it.Value)); }
	int totalGoldCount() const { return algo::sum(gold, RETLAMBDA(it.Value)); }
	int totalAncientCount() const { return totalBossCount() + totalGoldCount(); }
};

DungeonCounts countSubDungeons(const HyperMissionData& hyperData, const game::Tiles& tiles) {
	DungeonCounts out;
	std::unordered_set<int> seenDungeonInstanceIds;

	for (const auto* tile : tiles.getTiles()) {
		if (!seenDungeonInstanceIds.insert(tile->dungeon().instanceId()).second) {
			continue;
		}
		const auto dungeonId = tile->dungeon().def(). id;
		const auto dungeonType = hyperData.getAncientDungeonType(dungeonId);

		if (dungeonType == HyperMissionData::AncientDungeonType::Boss) {
			out.boss.FindOrAdd(stringutil::toFString(dungeonId.lowerId))++;
		}
		if (dungeonType == HyperMissionData::AncientDungeonType::Gold) {
			out.gold.FindOrAdd(stringutil::toFString(dungeonId.lowerId))++;
		}
	}
	return out;
}

FString getDungeonCountOutputWithTypes(const DungeonCounts& dungeonCounts) {
	TArray<FString> lines;

	const auto addTypes = [&](TMap<FString, int> types) {
		types.KeySort(std::less<FString>{});
		for (const auto& typeCount : types) {
			lines.Add(FString::Printf(TEXT("  %s (%d)"), *typeCount.Key, typeCount.Value));
		}
		if (types.Num()) {
			lines.Add("");
		}
	};
	lines.Add(FString::Printf(TEXT("Boss dungeons: %d"), dungeonCounts.totalBossCount()));
	addTypes(dungeonCounts.boss);
	lines.Add(FString::Printf(TEXT("Gold dungeons: %d"), dungeonCounts.totalGoldCount()));
	addTypes(dungeonCounts.gold);
	lines.Add(FString::Printf(TEXT("Total ancient dungeons: %d"), dungeonCounts.totalAncientCount()));

	return FString::Join(lines, TEXT("\n"));
}

void DoAncientProbabilityWithMobCountAndEnchantmentPoints(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	const auto maybeMobTypeCount = ArgAsInt(args, 0);
	const auto maybeEnchantmentPoints = ArgAsInt(args, 1);

	if (maybeMobTypeCount.IsSet() != maybeEnchantmentPoints.IsSet()) {
		out.Logf(ELogVerbosity::Error, TEXT("Either none or both of {ancient-mob-count, sacrificed-enchantment-points} must be set"));
		return;
	}

	const auto logProbability = [&](int mobTypeCount, int enchantmentsPoints) {
		const auto ancientProbability = missions::hajper::getAncientProbability(mobTypeCount, enchantmentsPoints);
		out.Logf(TEXT("mob-types: %2d, enchantment-points: %3d : %s (%s vs default)"),
			mobTypeCount,
			enchantmentsPoints,
			*valueformat::asSingleDecimalPercentage(ancientProbability.probability),
			*valueformat::asRelativeMultiplierPercentageChange(ancientProbability.probabilityMultiplier()));
	};

	out.Logf(TEXT("Probability that any special dungeon is an ancient mob dungeon (and percent increase over base case):"));
	if (maybeMobTypeCount && maybeEnchantmentPoints) {
		logProbability(maybeMobTypeCount.GetValue(), maybeEnchantmentPoints.GetValue());
	} else {
		for (const int enchantmentPoints : {0, 1, 2, 3, 10, 18}) {
			for (const int mobTypeCount : {1, 2, 3, 5, 13}) {
				logProbability(mobTypeCount, enchantmentPoints);
			}
		}
	}
}

struct SubMissionProviderState {
	int index;
};
using MaybeSubMissions = TOptional<TArray<FString>>;
using SubMissionProvider = std::function<MaybeSubMissions(SubMissionProviderState)>;

namespace submissionproviders {

SubMissionProvider randomOfLength(TArray<FString> ids, int count, int length, Random* rnd) {
	return [ids = std::move(ids), count, length, rnd](SubMissionProviderState s) -> MaybeSubMissions {
		return (s.index < count)? algo::map_tarray(Util::randomIndices(ids.Num(), FMath::Min(ids.Num(), length), rnd), RETLAMBDA(ids[it])) : MaybeSubMissions{};
	};
}

SubMissionProvider fromLevelIds(TArray<TArray<FString>> ids) {
	return [ids = std::move(ids)](SubMissionProviderState s) -> MaybeSubMissions {
		return ids.IsValidIndex(s.index) ? ids[s.index] : MaybeSubMissions{};
	};
}

SubMissionProvider fromLevelIds(TArray<FString> ids) {
	return fromLevelIds(TArray<TArray<FString>>{ids});
}

SubMissionProvider fromLevelIdsAndIndices(TArray<FString> ids, TArray<TArray<int>> indices) {
	return [ids = std::move(ids), indices = std::move(indices)](SubMissionProviderState s) -> MaybeSubMissions {
		return indices.IsValidIndex(s.index) ? algo::map_tarray(indices[s.index], RETLAMBDA(ids[it])) : MaybeSubMissions{};
	};
}

}

namespace internal {

template <typename T>
void getAllPermutations(const TArray<T>& items, int remaining, TArray<T>& picked, int mask, TArray<TArray<T>>& out) {
	if (!remaining) {
		out.Add(picked);
		return;
	}
	for (int i : range(items.Num())) {
		const int bitValue = 1 << i;
		if ((mask & bitValue) == 0) {
			picked.Add(items[i]);
			getAllPermutations(items, remaining - 1, picked, mask | bitValue, out);
			picked.Pop(false);
		}
	}
}

}

template <typename T>
TArray<TArray<T>> getAllPermutations(const TArray<T>& items, int count) {
	ensure(count <= items.Num());
	const int clampedCount = FMath::Clamp(count, 0, items.Num());

	constexpr int BitsInAnInt = sizeof(int) * 8;
	check(clampedCount <= BitsInAnInt);

	TArray<TArray<T>> out;
	TArray<T> current;
	internal::getAllPermutations(items, clampedCount, current, 0, out);
	return out;
}

struct StitchedIterationStat {
	ValidationIssues issues;
	int subMissionCount;
	RandomSeed seed;
	bool isLastObjectiveDefinitelyIncorrect;
	bool isSuccessful() const { return issues.empty() && !isLastObjectiveDefinitelyIncorrect; }
};

struct StitchedStat {
	TArray<FString> levelIds;
	Runs runs;
	TArray<StitchedIterationStat> iterationStats;

	bool isSuccessful() const {
		return getIterationCountWithMissingSubMissions() == 0 && algo::all_of(iterationStats, RETLAMBDA(it.isSuccessful()));
	}
	int getIterationCountWithMissingSubMissions() const { return algo::count_if(iterationStats, RETLAMBDA(it.subMissionCount < levelIds.Num())); }
};

struct StitchedStatPack {
	int stitchFailCount = 0;
	int stitchValidationFailCount = 0;
	Runs totalRuns;

	TArray<StitchedStat> nonSuccessfulStats;
};

struct StitchedRunState {
	const levelgen::Result& result;
};

using StitchedRunCallback = Pred<StitchedRunState>;

bool couldLastObjectiveBeCorrect(generatortest::GameState state) {
	const auto objectiveInits = game::objective::ObjectivesSystem(
		state.game,
		game::objective::create(state.levelDef.levelDef.data.objectives),
		&state.log
	).start_DEBUG_DO_NOT_USE();

	const int lastValidObjectiveIndex = objectiveInits.FindLastByPredicate(RETLAMBDA(it.passedValidation));
	return
		lastValidObjectiveIndex != INDEX_NONE &&
		objectiveInits[lastValidObjectiveIndex].info.type == io::ObjectiveType::Click &&
		objectiveInits[lastValidObjectiveIndex].info.rawName.Equals("name_the_escape", ESearchCase::IgnoreCase);
};

TOptional<StitchedStatPack> StitchCombinations(UWorld* world, const SubMissionProvider& subMissionProvider, int runsPerStitch, const FLevelSettings& settings, FOutputDevice& out, const StitchedRunCallback& callback, bool testObjectives) {
	const auto& mission = missions::get(settings.getLevelName());

	const auto hyperLevel = levelgen::sourcedata::hyperLevelFromFileSystem(mission.levelFilename());
	if (!hyperLevel) {
		return {};
	}

	const auto progressionKeys = levelsettingsutil::getProgressionKeys(world);
	const auto levelIds = algo::map_tarray(hyperLevel->levelIds, RETLAMBDA(stringutil::toFString(it.id)));
	const auto definitionLevelIds = algo::map_tarray(hyperLevel->definitionLevelIds, RETLAMBDA(stringutil::toFString(it.id)));

	StitchedStatPack outStatistics;
	int tooFewSubMissionTotalCount = 0;

	int index = 0;
	while (auto subMissions = subMissionProvider({ index++ })) {
		const levelgen::hajper::StitchInput input{
			subMissions.GetValue(),
			definitionLevelIds
		};
		auto stitchedLevel = levelgen::hajper::stitch(input);

		if (!stitchedLevel.data) {
			outStatistics.stitchFailCount++;
			continue;
		}

		const auto levelSettings = createLevelSettings({ mission.level(), progressionKeys });
		const auto levelGenData = levelgen::createLevelGenData(std::move(stitchedLevel.data), levelSettings, &out);

		if (!levelGenData) {
			outStatistics.stitchValidationFailCount++;
			continue;
		}

		StitchedStat stitchedStat{ input.orderedLevelIds };
		ValidationIssues currentIssues;
		TArray<int> subMissionCounts;

		const auto onLevelGenerated = [&](const levelgen::Result& result, bool isLastObjectiveDefinitelyIncorrect = false) {
			const auto subLevels = algo::map_as<std::set<CaseInsensitiveId>>(result.levelDef.stretches, [&](const generator::Stretch& it) {
				return CaseInsensitiveId(stringutil::toStdString(result.levelDef.data.dungeons[it.def.dungeonIndex].level.id));
			});
			const int subMissionCount = algo::count_if(subLevels, [&](const CaseInsensitiveId& id) {
				return algo::contains(result.levelDef.data.definedLevelIds, id);
			});
			stitchedStat.iterationStats.Add({
				result.issues,
				subMissionCount,
				result.finalSeed,
				isLastObjectiveDefinitelyIncorrect,
			});
			return callback({ result });
		};
		const auto onGameGenerated = [&](generatortest::GameState gameState) {
			return onLevelGenerated(gameState.result, testObjectives && !couldLastObjectiveBeCorrect(gameState));
		};

		if (testObjectives) {
			stitchedStat.runs = generateGame(levelGenData->runner(), mission.level(), onGameGenerated, runsPerStitch, *world, out);
		} else {
			stitchedStat.runs = generateLevelEx(levelGenData->runner(), mission.level(), onLevelGenerated, runsPerStitch);
		}
		outStatistics.totalRuns.successful += stitchedStat.runs.successful;
		outStatistics.totalRuns.failed += stitchedStat.runs.failed;

		if (!stitchedStat.isSuccessful()) {
			outStatistics.nonSuccessfulStats.Add(stitchedStat);
		}
	}

	return outStatistics;
}


static const FAutoConsoleCommand AncientProbabilityWithMobCountAndEnchantmentPointsCommand(TEXT("Dungeons.HyperMission.AncientProbabilityWithMobCountEnchantmentPoints")
	, TEXT("Check the probability for any dungeon to be an ancient mob dungeon: [ancient-mob-count sacrificed-enchantment-points]")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoAncientProbabilityWithMobCountAndEnchantmentPoints)
	, ECVF_Cheat);


namespace test {

struct BarChartDiagram {
	void add(int index, int count = 1) { mCounts[index] += count; }

	bool isEmpty() const  { return mCounts.empty(); }

	int minIndex() const { return isEmpty() ? 0 : mCounts.begin()->first; }
	int maxIndex() const { return isEmpty() ? 0 : mCounts.rbegin()->first; }

	int maxCount()   const { return isEmpty()? 0 : algo::max_element_by(mCounts, RETLAMBDA(it.second))->second; }
	int totalCount() const { return algo::sum(mCounts, RETLAMBDA(it.second)); }

	float average() const { return isEmpty()? 0 : algo::sum_with_init(mCounts, RETLAMBDA(it.first * it.second), 0.f) / totalCount(); }

	std::vector<int> indices() const { return algo::map_vector(mCounts, RETLAMBDA(it.first)); }
	TOptional<int> count(int index) const { const auto& it = mCounts.find(index); return it != mCounts.end() ? it->second : TOptional<int>{}; }
private:
	std::map<int, int> mCounts;
};

template <typename Collection>
BarChartDiagram createBarChartdiagram(const Collection& collection) {
	BarChartDiagram diagram;
	for (const auto& e : collection) {
		diagram.add(e);
	}
	return diagram;
}

void DoTestAncientDungeons(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	const auto hyperData = getHyperMissionData();
	if (!hyperData) {
		return;
	}

	TArray<DungeonCounts> dungeonCounts;
	const auto subMissionCount = missions::hajper::getAncientHuntNumSubMissions();

	Random rnd(ArgAsInt(commands, 1).Get(1));
	StitchCombinations(
		world,
		submissionproviders::randomOfLength(hyperData->levelIds, ArgAsInt(commands, 0).Get(100), subMissionCount, &rnd),
		10,
		createLevelSettings({ ELevelNames::netherhypermission, levelsettingsutil::getProgressionKeys(world) }),
		outputs::None(),
		[&](StitchedRunState state) {
			const auto tilesData = generatortest::util::createTiles(state.result);
			dungeonCounts.Add(countSubDungeons(hyperData.GetValue(), tilesData->tiles));
			return true;
		},
		false
	);
	struct Stat {
		BarChartDiagram diagram;
	};
	const auto getStat = [&](const auto& f) {
		return dungeonCounts.Num() ?
			Stat{ createBarChartdiagram(algo::map_vector(dungeonCounts, f)) } :
			Stat{};
	};
	const auto logStat = [&](const FString& dungeonTypeString, const Stat& stat) {
		out.Log(FString::Printf(TEXT("Average %s dungeon count: %.2f (min-max: %d-%d)"),
			*dungeonTypeString,
			stat.diagram.average(),
			stat.diagram.minIndex(),
			stat.diagram.maxIndex())
		);
	};

	const auto logDiagram = [&](const BarChartDiagram& diagram) {
		const float totalCount = diagram.totalCount();
		const float maxCount = diagram.maxCount();
		const int   maxWidth = 40;

		for (int i = 0, iMax = diagram.maxIndex(); i <= iMax; ++i) {
			const auto count = diagram.count(i).Get(0);
			const int x = count? FMath::Max(1, FMath::RoundToInt((count / maxCount) * maxWidth)) : 0;
			out.Logf(TEXT("%d %10.1f%% | %s"), i, 100*count/totalCount, *FString::ChrN(x, 'O'));
		}
	};

	const auto totalStat = getStat(RETLAMBDA(it.totalAncientCount()));
	logStat("special", totalStat);
	logDiagram(totalStat.diagram);
	out.Log(" \n");

	logStat("boss", getStat(RETLAMBDA(it.totalBossCount())));
	logStat("gold", getStat(RETLAMBDA(it.totalGoldCount())));
	out.Flush();
}

FString getStitchedStatsOutput(const TOptional<StitchedStatPack>& stats) {
	if (!stats) {
		return "Stitched FAIL: Couldn't read source data";
	}
	if (stats->nonSuccessfulStats.Num() == 0 && stats->stitchFailCount == 0 && stats->stitchValidationFailCount == 0 && stats->totalRuns.failed == 0) {
		return FString::Printf(TEXT("Stitched SUCCESS: %d levels were generated correctly"), stats->totalRuns.successful);
	}
	TArray<FString> lines;

	if (stats->stitchFailCount) {
		lines.Add(FString::Printf(TEXT("Failed to stitch level: %d times"), stats->stitchFailCount));
	}
	if (stats->stitchValidationFailCount) {
		lines.Add(FString::Printf(TEXT("Failed to validate stitched level: %d times"), stats->stitchValidationFailCount));
	}
	if (stats->totalRuns.failed) {
		lines.Add(FString::Printf(TEXT("Generation failed: %d of %d times"), stats->totalRuns.failed, (stats->totalRuns.failed + stats->totalRuns.successful)));
	}
	if (stats->nonSuccessfulStats.Num()) {
		TMap<TPair<FString, FString>, int> missionEndTooEarlyCounts;
		for (auto& stitchedStat : stats->nonSuccessfulStats) {
			for (auto& iterationStat : stitchedStat.iterationStats) {
				if (iterationStat.subMissionCount < stitchedStat.levelIds.Num()) {
					missionEndTooEarlyCounts.FindOrAdd(Util::makeTPair(
						stitchedStat.levelIds[iterationStat.subMissionCount - 1],
						stitchedStat.levelIds[iterationStat.subMissionCount]))++;
				}
			}
		}
		if (missionEndTooEarlyCounts.Num() > 0) {
			lines.Add("Too few sub-missions but generator may have done what it's supposed to:");
			missionEndTooEarlyCounts.ValueSort(std::greater<int>{});
			const auto maxCount = FMath::Min(5, missionEndTooEarlyCounts.Num());
			const auto totalTooEarlyCounts = algo::sum(missionEndTooEarlyCounts, RETLAMBDA(it.Value));
			int i = 0;
			for (auto& kv : missionEndTooEarlyCounts) {
				if (++i > maxCount) {
					break;
				}
				lines.Add(FString::Printf(TEXT("  %s -> %s : %.1f%%"), *kv.Key.Key, *kv.Key.Value, 100.0f * kv.Value / totalTooEarlyCounts));
			}
		}
		const auto allValidationIssues = algo::flatmap(stats->nonSuccessfulStats, [](const StitchedStat& stat) {
			return algo::flatmap(stat.iterationStats, RETLAMBDA(it.issues));
		});
		if (!allValidationIssues.empty()) {
			const auto validationIssuesSubset = game::util::randomSample(allValidationIssues, 10, Util::sharedRandom());
			const auto validationIssueCount = validationIssuesSubset.size();
			lines.Add(FString::Printf(TEXT("A subset of %d randomly picked validation issues:"), validationIssueCount));
			for (const auto& validationIssue : validationIssuesSubset) {
				lines.Add(stringutil::toFString(validationIssue.summary()));
			}
		}

		{
			TMap<FString, int> subMissionsWithFailedEndObjective;
			for (auto& stitchedStat : stats->nonSuccessfulStats) {
				if (const int errorCount = algo::count_if(stitchedStat.iterationStats, RETLAMBDA(it.subMissionCount == stitchedStat.levelIds.Num() && it.isLastObjectiveDefinitelyIncorrect))) {
					subMissionsWithFailedEndObjective.FindOrAdd(stitchedStat.levelIds.Last()) += errorCount;
				}
			}
			subMissionsWithFailedEndObjective.ValueSort(std::greater<int>{});
			if (subMissionsWithFailedEndObjective.Num()) {
				lines.Add("Sub-missions with failed end objectives:");
			}
			for (auto& subMissionCount : subMissionsWithFailedEndObjective) {
				const float percentOfTotal = 100.0f * subMissionCount.Value / stats->totalRuns.successful;
				lines.Add(FString::Printf(TEXT(" - %s (%.2f%% of all runs)"), *subMissionCount.Key, percentOfTotal));
			}
		}
	}
	return FString::Join(lines, TEXT("\n"));
}

void DoStitchAllCombinations(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	const auto hyperMissionData = getHyperMissionData();
	if (!hyperMissionData) {
		return;
	}
	const auto& levelIds = hyperMissionData->levelIds;
	const RandomSeed seed = ArgAsInt(commands, 0).Get(1);
	const auto subMissionCount = ArgAsInt(commands, 1).Get(2);

	if (subMissionCount > 3) {
		out.Logf(ELogVerbosity::Error, TEXT("Sub-mission count > 3 is not supported. Provided: %d"), subMissionCount);
		return;
	}

	out.Log(getStitchedStatsOutput(StitchCombinations(
		world,
		submissionproviders::fromLevelIdsAndIndices(
			levelIds,
			getAllPermutations(algo::iota_tarray<int>(levelIds.Num()), subMissionCount)),
		10,
		createLevelSettings({ ELevelNames::netherhypermission, levelsettingsutil::getProgressionKeys(world), seed }),
		outputs::None(),
		RETLAMBDA(true),
		true
	)));
}

void DoStitchSubMissions(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	out.Log(getStitchedStatsOutput(StitchCombinations(
		world,
		submissionproviders::fromLevelIds(commands),
		20,
		createLevelSettings({ ELevelNames::netherhypermission, levelsettingsutil::getProgressionKeys(world)}),
		outputs::None(),
		RETLAMBDA(true),
		true
	)));
}

const FAutoConsoleCommand CheckAncientSubDungeonCountCommand(TEXT("Test.Generator.AncientDungeons")
	, TEXT("")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoTestAncientDungeons)
	, ECVF_Cheat);

const FAutoConsoleCommand StitchAllCombinationsCommand(TEXT("Test.Generator.StitchAllCombinations")
	, TEXT("")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoStitchAllCombinations)
	, ECVF_Cheat);

const FAutoConsoleCommand StitchSubMissionsCommand(TEXT("Test.Generator.StitchSubMissions")
	, TEXT("")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoStitchSubMissions)
	, ECVF_Cheat);


}

namespace query {

void DoAncientDungeonsQuery(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	const auto hyperData = getHyperMissionData();
	if (!hyperData) {
		return;
	}
	const auto* game = actorquery::getGame(world);
	if (!game) {
		return;
	}
	out.Log(getDungeonCountOutputWithTypes(countSubDungeons(hyperData.GetValue(), game->tiles())));
}

const FAutoConsoleCommand AncientDungeonsQueryCommand(TEXT("Dungeons.Level.AHQuery.AncientDungeons")
	, TEXT("")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoAncientDungeonsQuery)
	, ECVF_Cheat);

}

}
