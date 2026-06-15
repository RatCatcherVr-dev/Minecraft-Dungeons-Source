/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/DiodeBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/material/Material.h"
#include "world/entity/Mob.h"
#include "world/Facing.h"
#include "world/Direction.h"
#include "world/redstone/Redstone.h"

bool DiodeBlock::isDiode(const Block& block) {
	return static_cast<const DiodeBlock*>(Block::mUnpoweredRepeater)->isSameDiode(block) || 
		(&block == Block::mUnpoweredComparator) || (&block == mPoweredComparator);
}

DiodeBlock::DiodeBlock(const std::string& nameId, int id, bool on)
	: Block(nameId, id, Material::getMaterial(MaterialType::Decoration))
	, mOn(on) {
	setSolid(false);
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

bool DiodeBlock::mayPlace(BlockSource& region, const BlockPos& pos) const {
	if (!region.canProvideSupport(pos.below(), Facing::UP, BlockSupportType::Any)) {
		return false;
	}
	
	return Block::mayPlace(region, pos);
}

bool DiodeBlock::canSurvive(BlockSource& region, const BlockPos& pos) const {
	if (!region.canProvideSupport(pos.below(), Facing::UP, BlockSupportType::Any)) {
		return false;
	}

	return Block::canSurvive(region, pos);
}

void DiodeBlock::onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const {
	if (region.getLevel().isClientSide() == false) {

		DataID data = region.getData(pos);
		BlockID current = region.getBlockID(pos);

		if (strength == 0) {
			if (current != getOffBlock()->mID) {
				region.setBlockAndData(pos, getOffBlock()->mID, data, UPDATE_ALL);
			}
		} else {
			if (current != getOnBlock()->mID) {
				region.setBlockAndData(pos, getOnBlock()->mID, data, UPDATE_ALL);
			}
		}
	}
}

int DiodeBlock::getVariant(int data) const {
	return mOn == true ? 1 : 0;
}

bool DiodeBlock::shouldRenderFace(BlockSource& region, const BlockPos& pos, FacingID face, const AABB& shape) const {
	if (face == Facing::DOWN || face == Facing::UP) {
		//	Up and down is a special case handled by the shape renderer.
		return false;
	}

	return true;
}

int DiodeBlock::getDirectSignal(BlockSource& region, const BlockPos& pos, int dir) const {
	return getSignal(region, pos, dir);
}

int DiodeBlock::getSignal(BlockSource& region, const BlockPos& pos, int dir) const {
	int data = region.getData(pos);
	if (!isOn(data)) {
		return Redstone::SIGNAL_NONE;
	}

	int direct = getBlockState(BlockState::Direction).get<int>(data);

	if ((direct == Direction::SOUTH && dir == Facing::SOUTH)
		|| (direct == Direction::WEST && dir == Facing::WEST)
		|| (direct == Direction::NORTH && dir == Facing::NORTH)
		|| (direct == Direction::EAST && dir == Facing::EAST)) {
		return getOutputSignal(region, pos, data);
	}

	return Redstone::SIGNAL_NONE;
}

bool DiodeBlock::isSignalSource() const {
	return true;
}

void DiodeBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	if (!canSurvive(region, pos)) {
		spawnResources(region, pos, region.getData(pos), 0);
		region.removeBlock(pos);
	}
}

bool DiodeBlock::isLocked(BlockSource& region, const BlockPos& pos, int data) const {
	return false;
}

bool DiodeBlock::isSameDiode(const Block& block) const {
	return &block == getOnBlock() || &block == getOffBlock();
}

int DiodeBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	return (((Math::floor(by.mRot.y * 4 / (360) + 0.5f)) & 3) + 2) % 4;
}

bool DiodeBlock::shouldPrioritize(BlockSource& region, const BlockPos& pos, int data) const {
	int dir = getBlockState(BlockState::Direction).get<int>(data);
	if (isDiode(region.getBlock(pos.offset(-Direction::STEP_X[dir], 0, -Direction::STEP_Z[dir])))) {
		int odata = region.getData(pos.offset(-Direction::STEP_X[dir], 0, -Direction::STEP_Z[dir]));
		int odir = getBlockState(BlockState::Direction).get<int>(odata);
		return odir != dir;
	}
	return false;
}

bool DiodeBlock::isOn(int data) const {
	return mOn;
}

bool DiodeBlock::shouldTurnOn(BlockSource& region, const BlockPos& pos, int data) const {
	return getInputSignal(region, pos, data) > Redstone::SIGNAL_NONE;
}

int DiodeBlock::getInputSignal(BlockSource& region, const BlockPos& pos, int data) const {
	return Redstone::SIGNAL_NONE;
}

bool DiodeBlock::isAlternateInput(const Block& block) const {
	return !block.isType(Block::mAir) && block.isSignalSource();
}

int DiodeBlock::getAlternateSignal(BlockSource& region, const BlockPos& pos, int data) const {
	int dir = getBlockState(BlockState::Direction).get<int>(data);

	switch (dir) {
	case Direction::SOUTH:
	case Direction::NORTH:
		return std::max(getAlternateSignalAt(region, pos.west(), Facing::WEST), getAlternateSignalAt(region, pos.east(), Facing::EAST));
	case Direction::EAST:
	case Direction::WEST:
		return std::max(getAlternateSignalAt(region, pos.south(), Facing::SOUTH), getAlternateSignalAt(region, pos.north(), Facing::NORTH));
	}

	return Redstone::SIGNAL_NONE;
}

int DiodeBlock::getAlternateSignalAt(BlockSource& region, const BlockPos& pos, int facing) const {
	const Block& block = region.getBlock(pos);
	if (isAlternateInput(block)) {
		if (block.isType(Block::mRedStoneDust)) {
			return region.getData(pos);
		} else {
			return block.getDirectSignal(region, pos, facing);
		}
	}

	return Redstone::SIGNAL_NONE;
}

int DiodeBlock::getOutputSignal(BlockSource& region, const BlockPos& pos, int data) const {
	return Redstone::SIGNAL_MAX;
}

int DiodeBlock::getTurnOffDelay(int data) {
	return getTurnOnDelay(data);
}
