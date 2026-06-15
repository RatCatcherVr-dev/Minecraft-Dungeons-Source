#pragma once

#include "world/level/BlockPos.h"
#include "BlockRegionTransform.h"

namespace blockpostransform {
	BlockPosTransform offset(BlockPos);
	BlockPosTransform create(const RegionSize& size, const BlockRegionTransform& transform);
	BlockPosTransform create(const RegionSize& size, const BlockRegionTransform& transform, const BlockPos& offset);
	BlockPosTransform _invert(const BlockPosTransform&);
	QuadrantAngle rotation(const BlockPosTransform&);
}
