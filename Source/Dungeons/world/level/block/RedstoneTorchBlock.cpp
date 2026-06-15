#include "Dungeons.h"

#include "world/level/block/RedstoneTorchBlock.h"

#include "world/Facing.h"
#include "world/item/ItemInstance.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/level/dimension/Dimension.h"
#include "world/redstone/Redstone.h"

RedstoneTorchBlock::RedstoneTorchBlock(const std::string& nameId, int id, bool on)
	: TorchBlock(nameId, id) 
	, mOn(on) {

	setTicking(false);

	mRenderLayer = BlockRenderLayer::RENDERLAYER_ALPHATEST_SINGLE_SIDE;	
}

void RedstoneTorchBlock::onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const {

	if (region.getLevel().isClientSide() == false) {

		DataID data = region.getData(pos);
		BlockID current = region.getBlockID(pos);

		if (strength == 0) {
			if (current != Block::mUnlitRedStoneTorch->mID) {
				region.setBlockAndData(pos, Block::mUnlitRedStoneTorch->mID, data, UPDATE_CLIENTS);
			}
		}
		else {
			if (current != Block::mLitRedStoneTorch->mID) {
				region.setBlockAndData(pos, Block::mLitRedStoneTorch->mID, data, UPDATE_CLIENTS);
			}
		}
	}
}

void RedstoneTorchBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	onLoaded(region, pos);
}

int RedstoneTorchBlock::getTickDelay() {
	return 2;
}

bool RedstoneTorchBlock::shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const {
	return true;
}

ItemInstance RedstoneTorchBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(Block::mLitRedStoneTorch);
}

ItemInstance RedstoneTorchBlock::getSilkTouchItemInstance(DataID data) const {
	return ItemInstance(Block::mLitRedStoneTorch);
}

int RedstoneTorchBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Block::mLitRedStoneTorch->mID;
}
