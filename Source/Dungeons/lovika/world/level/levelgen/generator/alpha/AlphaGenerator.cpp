#include "Dungeons.h"
#include "AlphaGenerator.h"
#include "game/LevelSettings.h"
#include "game/level/doors/DoorUtil.h"
#include "game/mission/hyper/DungeonEligibility.h"
#include "game/mission/hyper/HyperMissions.h"
#include "game/mission/hyper/HyperMissionUtil.h"
#include "game/mission/MissionDLCUtil.h"
#include "game/objective/TargetFinder.h"
#include "lovika/world/level/levelgen/TileGroup.h"
#include "lovika/world/level/levelgen/LevelGen.h"
#include "lovika/world/level/levelgen/generator/GeneratorUtil.h"
#include "lovika/world/level/levelgen/generator/GeneratorGraphUtil.h"
#include "lovika/world/level/levelgen/generator/GraphUtil.h"
#include "lovika/world/level/levelgen/generator/VolumesIntersector.h"
#include "lovika/world/level/levelgen/generator/alpha/internal/AlphaGeneratorTypes.h"
#include "lovika/world/level/levelgen/generator/alpha/internal/AlphaGeneratorHelpers.h"
#include "lovika/world/level/levelgen/generator/alpha/internal/AlphaGeneratorDeadEnds.h"
#include "lovika/world/level/levelgen/generator/alpha/internal/AlphaGeneratorDungeons.h"
#include "lovika/world/level/levelgen/generator/alpha/internal/AlphaGeneratorStrayPaths.h"
#include "lovika/world/level/levelgen/generator/alpha/internal/AlphaGeneratorTeleports.h"
#include "util/Algo.h"
#include "util/RandomUtil.h"
#include "util/FloatWeighedRandom.h"

namespace generator { namespace alpha {

//
// Tile placements
//
Either<PlaceResultVector, GsNoPlacement> getNonIntersectingTilePlacements(const TileGroup& tileGroup, const io::Stretch& stretch, const VolumesIntersector& intersector, const TOptional<TilePlacement>& base, const MetaTile& tile, int maxDoorWidthDifference, const PlacedTiles& doorChecker) {
	if (base) {
		// Main-path <exit> doors: If tile has defined 'exit-door's, it must be one of these. Else, any tile door.
		// Main-path <entry> doors: Can't use the doors declared as 'exit-door' (@note: the tile 'entry-door' is currently filtered out inside neighbourPlacements
		const auto& candidates = util::neighbourPlacements(util::exitDoorCandidates(tileGroup, base.GetValue()), tile, util::nonExitDoors(tile, tile.tile().doors()), maxDoorWidthDifference, doorChecker);
		if (candidates.empty()) {
			return GsNoPlacement(GsNoPlacement::Type::NoCandidates, stretch, base, tile);
		}
		const auto nonIntersecting = util::removeIntersecting(intersector, candidates);
		if (nonIntersecting.empty()) {
			return GsNoPlacement(GsNoPlacement::Type::IntersectsGeometry, stretch, base, tile);
		}
		return nonIntersecting;
	}
	return util::initialPlacements(tile);
};

using MaybePlaceResult = Either<PlaceResult, GsFailedPlacement>;

MaybePlaceResult getNextPlacement(const GenState& state) {
	if (auto nextTile = state.stretchDef().tileProvider(state)) {
		const auto base = state.placed.isEmpty() ? TOptional<TilePlacement>{} : state.placed.back(); //@placed
		const auto maxDoorWidthDifference = state.env.config.maxDoorWidthDifference();
		const auto tilePlacements = getNonIntersectingTilePlacements(state.env.tileGroup, state.stretchDef().stretch, state.intersector, base, nextTile.GetValue(), maxDoorWidthDifference, state.placed);

		// do not repeat the same tile-and-rotation
		// is there a better place to verify this?
		if (tilePlacements.failed) {
			return GsFailedPlacement{ {}, tilePlacements.failed.GetValue() };
		} else if (state.placed.isEmpty()) { //@placed
			return *Util::randomChoice(tilePlacements.success.GetValue(), &state.rnd);
		} else {
			const auto& previous = state.placed.back();
			// try to at least not repeat the same tile and rotation
			auto placementsWithNewTileOrRotation = util::filterIfSome(tilePlacements.success.GetValue(), [&previous](const TilePlacement& tilePlacement) {
				return previous.tile().id() != tilePlacement.tile().id() || previous.placement().rotation != tilePlacement.placement().rotation;
			});
			return *Util::randomChoice(placementsWithNewTileOrRotation, &state.rnd);
		}
	}
	return GsFailedPlacement{ GsNoTile(state.stretchDef().stretch) };
}

MaybeGenResult startSafe(const Env& env, const std::vector<io::Stretch>& ioStreches, RandomSeed seed, int maxPops) {
	Unique<GenState> state = std::make_unique<GenState>(env, ioStreches, seed);
	GenStateRunner runner(*state);
	GeneratorStuck failedPlacements;
	
	int lastStuckIndex = 0;
	int stuckCount = 0;
	int totalPops = 0;

	while (!runner.isDone()) {
		if (state->stretchDef().isDone(*state)) {
			runner.goToNextStretch();
			continue;
		}
		MaybePlaceResult placement = getNextPlacement(*state);

		if (placement.success) {
			runner.push(placement.success.GetValue());
		} else {
			failedPlacements.add(placement.failed.GetValue());

			if (++totalPops >= maxPops) {
				return GenerationError{ failedPlacements };
			}

			int currentStuckIndex = state->globalTileIndex;
			if (currentStuckIndex == lastStuckIndex) {
				++stuckCount;
			}
			lastStuckIndex = currentStuckIndex;

			if (stuckCount >= 20) {
				runner.pop(8); // 8 is fairly high, but it's probably needed with all transition tiles
				stuckCount = 0;
			} else {
				runner.pop();
			}
		}
	}
	runner.setMainPathDone();
	generateStrayPaths(*state);
	generateDeadEnds(*state);
	generateDungeonsBatchHACK(*state);
	generateDungeons(*state);
	return std::move(state);
}


MaybeGenResult generateSafe(const Env& env, const std::vector<io::Stretch>& ioStreches, RandomSeed seed, int tries, int maxFailedPlacements) {
	GenerationError error;
	
	while (--tries >= 0) {
		auto state = startSafe(env, ioStreches, seed, maxFailedPlacements);

		if (state.success) {
			return std::move(state.success);
		} else {
			check(state.failed.IsSet());
			error.append(state.failed.GetValue());
		}
		seed++;
	}
	return error;
}

template <typename T, typename Func>
void forEachError(const std::unordered_map<T, size_t>& countMap, const Func& f) {
	if (countMap.empty()) {
		return;
	}
	std::vector<std::pair<T, size_t>> values(countMap.begin(), countMap.end());
	std::sort(values.begin(), values.end(), [](auto& a, auto& b) { return a.second > b.second; });

	for (auto& kv : values) {
		f(kv.first, kv.second);
	}
}


TOptional<Validation> generateGeneratorStuckValidation(const Env& env, const GeneratorStuck& stuck) {
	if (stuck.isEmpty()) {
		return {};
	}
	Validation validation("Generator got stuck");

	const auto error = [&validation, &env, errorCount = stuck.errorCount()](float fractionThreshold, size_t count, const GsStretch& stretch, const std::string& s) {
		const float fraction = static_cast<float>(count) / errorCount;
		if (fraction < fractionThreshold) {
			return;
		}
		char percentBuf[32];
		sprintf(percentBuf, "%.1f%% ", 100.0f * fraction);

		const std::string stretchDesc = "stretch: '" + stretch.id + '@' + env.dungeons[stretch.dungeonIndex].id.id + "' (index: " + std::to_string(1 + stretch.index) + ')';
		validation.error(percentBuf + s + " in " + stretchDesc);
	};

	forEachError(stuck.noTiles, [&error](const GsNoTile& it, size_t count) {
		error(0, count, it.stretch, "Couldn't find any tile to use");
	});

	for (float errorThreshold = 0.03f; errorThreshold > 0.0001f && validation.errors().empty(); errorThreshold /= 2) {
		forEachError(stuck.noPlacements, [&error, errorThreshold](const GsNoPlacement& it, size_t count) {
			error(errorThreshold, count, it.stretch, it.toString());
		});
	}
	return validation;
}

ValidationIssues generateValidationIssuesFor(const Env& env, const GenerationError& errors) {
	ValidationIssues issues;

	if (auto stuckValidation = generateGeneratorStuckValidation(env, errors.stuck)) {
		issues.push_back(stuckValidation.GetValue());
	}
	append(issues, errors.issues);
	return issues;
}

Result generate(Env env, const std::vector<io::Stretch>& initialDungeon, RandomSeed seed, int maxTries) {
	auto mainState = std::make_unique<GenState>(env, std::vector<io::Stretch>{}, seed);

	bool first = true;
	std::vector<graph::Edge> temporaryJumpConnections;
	for (auto& stretches : splitStretchesIntoJumpConnectedGroups(env.tileGroup, initialDungeon)) {
		const auto state = generateSafe(env, stretches, seed, maxTries, 1000);
		if (state.success) {
			if (!first) {
				temporaryJumpConnections.push_back(graph::util::fakeEdge(graph::util::findEndTileIndex(mainState->makeResult().tileInfos), mainState->placed.size()));
			}
			mainState->placeOffsite({}, *state.success); // Preferably we connect the doors later
			append(mainState->issues, state.success->issues);
			first = false;
		} else {
			return Result(generateValidationIssuesFor(env, state.failed.GetValue()));
		}
	}
	return connectJumpDoorsAndMakeResult(*mainState, temporaryJumpConnections);
}

//
// Generator interfaces
//
namespace internal {

using namespace levelgen::hajper;

GeneratorFunc GeneratorWithDungeonSelector(DungeonSelector dungeonSelector, DungeonBatchSelector dungeonBatchSelector) {
	return [dungeonSelector, dungeonBatchSelector](const TileGroup& tileGroup, const io::Level& level, RandomSeed seed, int maxTries) {
		Random rnd(seed);
		Env env{ tileGroup, level.dungeons, level.levelGenConfig, dungeonSelector, dungeonBatchSelector };
		return generate(std::move(env), util::initialDungeon(level, rnd).stretches, seed, maxTries);
	};
}

TArray<int> getRandomIndicesForHyperDungeonTeleports(DungeonBatchSelectorState state, int targetCount) {
	const auto filteredIndices = algo::copy_if(algo::iota_tarray<int>(state.teleportDoorDefs.Num()),
		RETLAMBDA(state.teleportDoorDefs[it].def.hasDungeonId(ids::HyperDungeon())));

	return game::util::randomSample(filteredIndices, targetCount, state.rnd, true);
}

DungeonBatchSelector createHyperDungeonBatchSelector(DungeonSelector selector) {
	return [selector = std::move(selector)] (DungeonBatchSelectorState state) {
		std::map<int, CaseInsensitiveId> out;

		const auto indices = [&] {
			const int targetDungeonCount = 2;
			return getRandomIndicesForHyperDungeonTeleports(state, targetDungeonCount);
		}();

		for (int i : indices) {
			if (auto selectedId = selector({ ids::HyperDungeon(), state.teleportDoorDefs[i].def, state.rnd })) {
				out[i] = selectedId.GetValue();
			}
		}
		return out;
	};
};

GeneratorFunc HyperGenerator(const io::HyperLevel& hyperLevel, const io::HyperDungeonPredicate& dungeonPredicate, int sacrificedTotalEnchantmentPoints) {
	class SpecialDungeonPicker {
	public:
		SpecialDungeonPicker(const std::vector<io::HyperDungeon>& dungeons, int sacrificedTotalEnchantmentPoints)
			: mGoldDungeons(algo::copy_if_map_vector(dungeons, RETLAMBDA(it.isGoldDungeon), RETLAMBDA(it.id)))
			, mAncientDungeons(algo::copy_if_map_vector(dungeons, RETLAMBDA(!it.isGoldDungeon), RETLAMBDA(WeightedId(it.id, 1))))
			, mAncientDungeonProbability(missions::hajper::getAncientProbability(mAncientDungeons.size(), sacrificedTotalEnchantmentPoints).probability) {
		}

		TOptional<CaseInsensitiveId> operator()(DungeonSelectorState state) const {
			if (state.inputId != ids::HyperDungeon()) {
				return {};
			}
			if (!mAncientDungeons.empty() && state.rnd.testProbability(mAncientDungeonProbability)) {
				return *FloatWeighedRandom::getRandomItem(&state.rnd, mAncientDungeons);
			}
			if (!mGoldDungeons.empty()) {
				return *FloatWeighedRandom::getRandomItem(&state.rnd, mGoldDungeons);
			}
			return {};
		}
	private:
		std::vector<WeightedId> mGoldDungeons;
		std::vector<WeightedId> mAncientDungeons;
		float mAncientDungeonProbability;
	};

	const SpecialDungeonPicker dungeonSelector(
		algo::copy_if(hyperLevel.hyperDungeons, dungeonPredicate),
		sacrificedTotalEnchantmentPoints
	);

	return GeneratorWithDungeonSelector(
		[](DungeonSelectorState s) { return s.inputId; },
		createHyperDungeonBatchSelector(dungeonSelector)
	);
};

}

const GeneratorFunc& DefaultGenerator() {
	static const GeneratorFunc generator = internal::GeneratorWithDungeonSelector([](DungeonSelectorState s) { return s.inputId; }, {});
	return generator;
}

GeneratorFunc HyperGenerator(const io::HyperLevel& hyperLevel, const FLevelSettings& levelSettings) {
	return internal::HyperGenerator(
		hyperLevel,
		missions::hajper::getHyperDungeonLevelGenerationPredicate(levelSettings.missionState),
		levelSettings.missionState.getTotalEnchantmentPoints()
	);
}

}}
