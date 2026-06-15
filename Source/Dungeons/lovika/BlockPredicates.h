#pragma once
#include "world/level/BlockPos.h"
#include "world/level/block/Block.h"

struct FullBlock;
struct BlockID;

namespace blockpredicates {

BlockPredicate isBlock(FullBlock block);
BlockPredicate isBlock(BlockID block);
BlockPredicate isBlock(Block* block);

BlockPredicate negative(const BlockPredicate& predicate);
}
