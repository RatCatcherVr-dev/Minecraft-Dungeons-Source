#pragma once

#include "world/level/BlockPos.h"
#include "lovika/io/WalkableHeight.h"

class WalkableCell {
public:
	WalkableCell(const BlockPos&, WalkableHeight);

	BlockPos position;
	WalkableHeight value;
};

WalkableCell              transformed(const WalkableCell&, const BlockPosTransform&);
std::vector<WalkableCell> transformed(const std::vector<WalkableCell>&, const BlockPosTransform&);
TOptional<Height>         transformed(TOptional<Height>, const BlockPosTransform&);
