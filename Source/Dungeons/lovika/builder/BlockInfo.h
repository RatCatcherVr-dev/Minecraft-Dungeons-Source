#pragma once

#include "CommonTypes.h"
#include "world/level/BlockPos.h"

namespace builder {

struct BlockInfo {
	BlockInfo(FullBlock fullBlock, BlockPos pos)
		: fullBlock(fullBlock), pos(pos) {}

	FullBlock fullBlock;
	BlockPos pos;
};

}