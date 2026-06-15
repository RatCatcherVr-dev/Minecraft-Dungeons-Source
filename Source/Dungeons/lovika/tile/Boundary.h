#pragma once

#include "world/level/BlockPos.h"
#include "lovika/BlockCuboid.h"

class Boundary {
public:
	Boundary(const BlockPos&, int height);

	BlockCuboid toBlockCuboid() const { return BlockCuboid::fromPositionAndSize(position, 1, height, 1); }

	BlockPos position;
	int height;
};

Boundary transformed(const Boundary&, const BlockPosTransform&);
std::vector<Boundary> transformed(const std::vector<Boundary>&, const BlockPosTransform&);

std::vector<BlockCuboid> mergeBoundaries(std::vector<Boundary>);
