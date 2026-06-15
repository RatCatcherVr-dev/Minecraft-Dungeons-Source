#pragma once

#include "CommonTypes.h"
#include "LevelDef.h"
#include "lovika/io/LevelFile.h"
#include "lovika/world/level/levelgen/decorator/PropDecorator.h"
#include "lovika/world/level/levelgen/metascore/MetaScoreTypes.h"
#include "util/Validation.h"

namespace generator { struct Result; }

namespace levelgen {

struct Result {
	bool success;
	ValidationIssues issues;

	LevelDef levelDef;
	RandomSeed finalSeed;
};

class Runner {
public:
	Runner(TileGroup tiles, TileGroup props, io::Level, generator::GeneratorFunc);

	Result run(RandomSeed, const TOptional<score::MetaScorer>& = {}, int maxTries = 100) const;
private:
	Result runOnce(RandomSeed, int maxTries = 100) const;
	Result runMeta(RandomSeed, const score::MetaScorer&, int maxTries = 100) const;

	Result _buildResult(const generator::Result&) const;

	io::Level mLevelFile;
	TileGroup mTiles;
	TileGroup mProps;
	generator::GeneratorFunc mGenerator;
	decorator::PropDecorator mDecorator;
};

}
