#pragma once

#include "PostProcessTypes.h"
#include "lovika/tile/TileArea.h"

class BlockSource;

namespace levelgen { struct LevelDef; }

namespace postprocess {

struct Result {
	std::vector<TileArea> tileAreas;
	std::vector<BlockCuboid> filledDoors;
};

Result run(BlockSource&, const levelgen::LevelDef&, const Config&);

}
