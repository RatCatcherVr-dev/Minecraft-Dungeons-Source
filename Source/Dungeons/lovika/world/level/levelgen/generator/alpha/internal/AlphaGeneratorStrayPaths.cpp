#include "Dungeons.h"
#include "AlphaGeneratorStrayPaths.h"
#include "AlphaGeneratorDeadEnds.h"
#include "lovika/tile/TilePredicates.h"
#include "lovika/world/level/levelgen/TileGroup.h"
#include "lovika/world/level/levelgen/generator/GeneratorUtil.h"
#include "util/FloatWeighedRandom.h"
#include "util/RandomUtil.h"

namespace generator { namespace alpha {

bool tryPlaceTile(GenState& state, const TilePlacement& connectedTo, const StretchTileProvider& tileProvider, StretchId stretchId, int tries = 32) {
	for (int t = 0; t < tries; ++t) {
		const auto nextTile = tileProvider(state);
		if (!nextTile) {
			break;
		}
		const auto maxDoorWidthDifference = state.env.config.maxDoorWidthDifference();
		// Stray-path (entry) doors can be placed on all remaining doors (@note: the tile 'entry-door' is currently filtered out inside neighbourPlacements
		const auto candidates = util::neighbourPlacements(connectedTo.doors(), nextTile.GetValue(), nextTile->tile().doors(), maxDoorWidthDifference, state.placed);
		const auto placements = util::removeIntersecting(state.intersector, candidates);
		if (placements.empty()) {
			continue;
		}
		state.place(std::move(*Util::randomChoice(placements, &state.rnd)), copyAsOffMainPathStretchId(stretchId));
		return true;
	}
	return false;
}

int generateStrayPath(GenState& state, const StretchTileProvider& tileProvider, TilePlacement connectedTo, StretchId stretchId, int targetLength) {
	int i;
	for (i = 0; i < targetLength; ++i) {
		if (tryPlaceTile(state, connectedTo, tileProvider, stretchId)) {
			connectedTo = state.placed.back();
		} else {
			break;
		}
	}
	return i;
}

io::StrayPathConfig createStrayPathConfigMergedWithGeneratorDefaults(const io::Stretch& stretch) {
	const io::StrayPathVariant fallback("Variant_alphaGeneratorDefault", 1.0f, io::Interval{ 1, 5 }, {} /*stretch.tiles*/, {} /*, deadEnds are handled later */);
	return io::merge(io::StrayPathConfig{ 0.3f, fallback }, stretch.overrides.strayPath);
}

static void handleStrayPaths(GenState& state, size_t i, const io::StrayPathConfig& config, const TileGroup& globalLevelDeadEnds) {
	if (!state.rnd.testProbability(config.probability.GetValue()) || config.variants->empty()) {
		return;
	}
	const auto& variant = *FloatWeighedRandom::getRandomItem(&state.rnd, config.variants.GetValue());
	if (!variant.tiles) {
		return;
	}
	if (const auto targetStrayPathLength = state.rnd.nextInt(variant.maxLength->min, variant.maxLength->max + 1)) {
		const StretchTileProvider& tileProvider = tilegetters::Default(createMetaTiles(state.env.tileGroup, variant.tiles.GetValue()));
		if (const auto generatedLength = generateStrayPath(state, tileProvider, state.placed[i], state.stretchIds[i], targetStrayPathLength)) {
			const auto& deadEnds = variant.deadEnds ? createMetaTiles(state.env.tileGroup, variant.deadEnds.GetValue()) : globalLevelDeadEnds.tiles();

			for (int j = state.placed.size() - generatedLength, limit = state.placed.size(); j < limit; ++j) {
				generateDeadEnds(state, j, deadEnds); // Place stray-path specific dead-ends on all tiles this stray-path
			}
		}
	}
}

void generateStrayPaths(GenState& state) {
	const auto mergedStrayPathConfigs = algo::map_vector(state.stretches, RETLAMBDA(createStrayPathConfigMergedWithGeneratorDefaults(it.stretch)));
	const int mainPathTilesCount = state.placed.size();
	DeadEndsPerLevel deadEndsPerLevel(state.env);

	for (int i = 0; i < mainPathTilesCount; ++i) {
		auto freeDoors = util::freeDoors(state.placed[i].doors(), state.placed);
		if (freeDoors.empty()) {
			continue;
		}
		const auto& stretchId = state.stretchIds[i]; // @graph?
		handleStrayPaths(state, i, mergedStrayPathConfigs[stretchId.index], deadEndsPerLevel.getOrCreate(state.dungeonForTileIndex(i)->level));
	}
}

}}
