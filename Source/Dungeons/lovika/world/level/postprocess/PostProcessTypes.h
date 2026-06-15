#pragma once

#include <functional>
#include "CommonTypes.h"
#include "HeightMapper.h"
#include "lovika/world/level/LevelGenRandom.h"

class BlockPos;

namespace postprocess {

namespace worldfill {
	struct State {
		HeightMapperSpan span;
		BlockPos pos;
		LevelGenRandom& rnd;
	};
	using BlockProvider = std::function<FullBlock(const State&)>;
}

namespace door {
	struct State {
		worldfill::State data;
		const std::vector<FullBlock>& frameBlocks;
	};
	using BlockProvider = std::function<FullBlock(const State&)>;
}

struct Config {
	Config(worldfill::BlockProvider, door::BlockProvider);
	Config(const worldfill::BlockProvider&);

	worldfill::BlockProvider outsideBlockProvider;
	door::BlockProvider doorBlockProvider;
};

}
