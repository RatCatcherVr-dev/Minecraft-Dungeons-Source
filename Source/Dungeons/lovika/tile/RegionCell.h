#pragma once

#include "world/level/BlockPos.h"

class RegionCell {
public:
	RegionCell(const BlockPos&, uint8_t);

	BlockPos position;
	uint8_t value;
};

RegionCell transformed(const RegionCell&, const BlockPosTransform&);
std::vector<RegionCell> transformed(const std::vector<RegionCell>&, const BlockPosTransform&);
