/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/DetectorRailBlock.h"
#include "world/level/Level.h"
#include "world/level/BlockSource.h"
#include "world/level/dimension/Dimension.h"
#include "world/redstone/Redstone.h"
#include "world/entity/Entity.h"

DetectorRailBlock::DetectorRailBlock(const std::string& nameId, int id) 
	: BaseRailBlock(nameId, id, true) {
	setTicking(false);
}

int DetectorRailBlock::getTickDelay() const {
	return 20;
}

int DetectorRailBlock::getVariant(int data) const {
	return Block::mDetectorRail->getBlockState(BlockState::RailDataBit).get<int>(data);
}

bool DetectorRailBlock::shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const
{
	return true;
}

void DetectorRailBlock::tick(BlockSource& region, const BlockPos& pos, Random& random) const {
	int data = region.getData(pos);

	if (region.getLevel().isClientSide() || Block::mDetectorRail->getBlockState(BlockState::RailDataBit).get<int>(data) == 0) {
		return;
	}

	checkPressed(region, pos, data);
}

bool DetectorRailBlock::hasComparatorSignal() const {
	return true;
}

int DetectorRailBlock::getComparatorSignal(BlockSource& region, const BlockPos& pos, FacingID dir, int data) const {
	return Redstone::SIGNAL_NONE;
}

void DetectorRailBlock::onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const {
	//Do nothing.. the Base Rail has action needed, but not needed here
}

void DetectorRailBlock::checkPressed(BlockSource& region, const BlockPos& pos, int state) const {
	EntityList entities = region.getEntities(EntityType::Minecart, getSearchBB(pos));
	bool shouldBePressed = !entities.empty();

	handlePressed(region, pos, state, shouldBePressed);
}

void DetectorRailBlock::handlePressed(BlockSource& region, const BlockPos& pos, int state, bool shouldBePressed) const {
	auto &railDataState = Block::mDetectorRail->getBlockState(BlockState::RailDataBit);
	bool wasPressed = railDataState.get<int>(state) != 0;

	DataID data = state;
	if (shouldBePressed && !wasPressed) {
		railDataState.set(data, 1);
		region.setBlockAndData(pos, mID, data, Block::UPDATE_ALL);
	}
	if (!shouldBePressed && wasPressed) {
		railDataState.set(data, 0);
		region.setBlockAndData(pos, mID,data, Block::UPDATE_ALL);
	}
}

AABB DetectorRailBlock::getSearchBB(BlockPos pos) const {
	float b = 0.2f;

	return AABB(pos.x + b, (float)pos.y, pos.z + b, pos.x + 1 - b, pos.y + 1 - b, pos.z + 1 - b);
}


