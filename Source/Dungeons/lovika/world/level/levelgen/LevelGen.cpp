#include "Dungeons.h"
#include "LevelGen.h"
#include "TileSet.h"
#include "game/mobspawn/MobGroupUtil.h"
#include "lovika/world/level/LevelGenRandom.h"
#include "lovika/world/level/levelgen/generator/GeneratorGraphUtil.h"
#include "lovika/world/level/levelgen/metascore/MetaScoreTypes.h"
#include "lovika/world/level/levelgen/generator/alpha/AlphaGenerator.h"

namespace levelgen {

Runner::Runner(TileGroup tiles, TileGroup props, io::Level levelFile, generator::GeneratorFunc generate)
	: mLevelFile(std::move(levelFile))
	, mTiles(std::move(tiles))
	, mProps(std::move(props))
	, mGenerator(std::move(generate))
	, mDecorator(mProps)
{
	checkf(!mLevelFile.objectGroups.empty(), TEXT("Must have at least one <object-group>"));
}

Result Runner::runOnce(RandomSeed seed, int maxTries) const {
	return _buildResult(mGenerator(mTiles, mLevelFile, seed, maxTries));
}

Result Runner::runMeta(RandomSeed seed, const score::MetaScorer& metaScorer, int maxTries) const {
	auto scoreRnd = std::make_unique<LevelGenRandom>(seed);
	auto pickRnd  = std::make_unique<LevelGenRandom>(seed);

	score::Iterations iterations;
	std::vector<score::SeedScore> successfulScores;
	ValidationIssues issues;

	auto seedGenerator = [nextSeed = seed, rnd = LevelGenRandom(seed)]() mutable {
		const auto out = nextSeed;
		nextSeed = rnd();
		return out;
	};

	for (int tries = 1; tries <= maxTries; ++tries) {
		const auto genResult = mGenerator(mTiles, mLevelFile, seedGenerator(), 1);

		const score::SubRange subRange(genResult);
		const auto maybeScore = genResult.success() ? metaScorer.scorer({ genResult, subRange, *scoreRnd }) : TOptional<float>{};

		if (maybeScore) {
			iterations.successful++;
			successfulScores.push_back({ genResult.seed, maybeScore.GetValue() });

		} else if (genResult.success()) {
			iterations.failedScoring++;

		} else {
			iterations.failedGeneration++;
			append(issues, genResult.issues);
		}

		const bool isLastIteration = (tries >= maxTries);
		if (maybeScore || (isLastIteration && !successfulScores.empty())) {
			if (auto pickResult = metaScorer.picker({ iterations, successfulScores, isLastIteration, *pickRnd })) {
				return runOnce(pickResult.GetValue(), 1);
			}
		}
	}
	if (iterations.failedScoring) {
		issues.push_back(validationErrors("Level MetaScoring didn't pass any levels", {
			"MetaScoring discarded " + std::to_string(iterations.failedScoring) +
			" out of " + std::to_string(iterations.totalGeneratedLevels()) + " generated levels" }));
	}
	return { false, issues };
}

Result Runner::run(RandomSeed seed, const TOptional<score::MetaScorer>& metaScorer, int maxTries) const {
	if (metaScorer) {
		return runMeta(seed, metaScorer.GetValue(), maxTries);
	}
	return runOnce(seed, maxTries);
}

Result Runner::_buildResult(const generator::Result& genResult) const {
	if (!genResult.success()) {
		return { false, genResult.issues };
	}
	LevelDef levelDef{
		mLevelFile.id,
		genResult.stretches,
		genResult.tileInfos,
		generator::graph::util::createProgress(genResult.graph, genResult.tileInfos),
		genResult.graph,
		mLevelFile.passThrough
	};
	decorator::decorate(levelDef, mDecorator, genResult.seed);
	translateTo(levelDef, BlockPos(32, 20, 32));

	for (auto& stretch : levelDef.stretches) {
		game::mobspawn::buildPredicates(stretch.def.mobs);
	}
	return { true, genResult.issues, levelDef, genResult.seed };
}

}
