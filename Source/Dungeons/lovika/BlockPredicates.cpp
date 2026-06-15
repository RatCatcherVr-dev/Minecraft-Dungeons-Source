#include "Dungeons.h"
#include "BlockPredicates.h"

namespace blockpredicates {

BlockPredicate isBlock(FullBlock block) {
	return [block](auto pos, auto _block) { return _block == block; };
}
BlockPredicate isBlock(BlockID block) {
	return [block](auto pos, auto _block) { return _block.id == block; };
}

BlockPredicate isBlock(Block* block) {
	return[id = block->mID](auto pos, auto _block) { return _block.id == id; };
}

BlockPredicate negative(const BlockPredicate& predicate) {
	return [&predicate](auto pos, auto block) {
		return !predicate(pos, block);
	};
}

}
