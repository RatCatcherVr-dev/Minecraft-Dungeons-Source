#include "Dungeons.h"
#include "OutsideFiller.h"
#include "HeightMapper.h"
#include "world/level/BlockSource.h"
#include "lovika/tile/PlacedTiles.h"
#include "lovika/world/level/LevelGenRandom.h"
#include "HeightMapper.h"

namespace postprocess { namespace worldfill {

std::vector<TileArea> fill(BlockSource& region, const PlacedTiles& doorQuery, const PlacementVector& placements, const BlockProvider& blockProvider) {
	if (!blockProvider) { //@todo: should still do the interior size calculation
		return HeightMapper(region, doorQuery, placements, 0).tileAreas();
	}
	
	HeightMapper heightMapper(region, doorQuery, placements, NumBorderBlocks);

	LevelGenRandom rnd(1);
	State state{ {0,0,0}, BlockPos::ZERO, rnd };

	for (auto pos : BlockPosIteration::range(heightMapper.bounds2D())) {
		auto span = heightMapper.get(pos.x, pos.z);
		if (span.distance > NumBorderBlocks) {
			continue;
		}
		state.span = span;
		state.pos = pos;
		for (state.pos.y = span.min; state.pos.y < span.max; ++state.pos.y) {
			region.setBlockAndData(state.pos, blockProvider(state), Block::UPDATE_NONE);
		}
	}
	return heightMapper.tileAreas();
}

}}
