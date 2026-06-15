/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/HeavyBlock.h"
#include "world/level/material/Material.h"
#include "world/level/Level.h"
#include "world/level/BlockSource.h"
#include "legacy/Core/Math/Color.h"

HeavyBlock::HeavyBlock(const std::string& nameId, int id, const Material& material) :
	Block(nameId, id, material) {
	mCanSlide = true;
	mHeavy = true;
}

BlockPos HeavyBlock::_findBottomSlidingBlock(BlockSource& region, const BlockPos& start) const {
	BlockPos pos = start;
	for (; pos.y > 0; --pos.y) {

		const Block& block = region.getBlock(pos);

		if (!block.canSlide()) {
			break;
		}
	}

	return pos.above();
}

bool HeavyBlock::isFree(BlockSource& region, const BlockPos& pos) const {
	const Block* t = Block::mBlocks[region.getBlockID(pos)];

	if (t == nullptr || t->isType(Block::mAir)) {
		return true;
	}

	if (t == (const Block*)Block::mFire) {
		return true;
	}

	const Material& material = t->getMaterial();
	if (material.isType(MaterialType::Air)) {
		return true;
	}

	if (material.isType(MaterialType::Water)) {
		return true;
	}

	if (material.isType(MaterialType::Lava)) {
		return true;
	} else{
		return false;
	}
}

void HeavyBlock::checkSlide(BlockSource& region, const BlockPos& pos) const {
	BlockPos bottom = _findBottomSlidingBlock(region, pos);
	if (bottom.y <= 0) {
		return;
	}

	if (isFree(region, bottom.below())) {
		bool isCreative = region.getLevel().getGameType() == GameType::Creative;
		startFalling(region, bottom, &region.getBlock(bottom), isCreative);
	}
}

int HeavyBlock::getTickDelay() const {
	return 2;
}

void HeavyBlock::tick(BlockSource& region, const BlockPos& pos, Random& random) const {
	if (!region.getLevel().isClientSide()) {
		checkSlide(region, pos);
	}
}

void HeavyBlock::onLand(BlockSource& region, const BlockPos& pos) const {
}

bool HeavyBlock::falling() const {
	return false;
}
