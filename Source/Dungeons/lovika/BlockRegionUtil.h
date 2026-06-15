#pragma once

#include "world/level/BlockPos.h"

class BlockRegion;
class BlockRegionTransform;

class BlockRegionUtil {
public:
	static Unique<BlockRegion> cloneWith(const BlockRegion&, const BlockRegionTransform&);

	static void for_each(const BlockRegion&, const BlockCallback&);
	static void for_each_matching(const BlockRegion&, const BlockPredicate&, const BlockCallback&);
};
