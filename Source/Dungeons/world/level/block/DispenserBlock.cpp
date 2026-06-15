/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/DispenserBlock.h"
#include "world/level/material/Material.h"
#include "world/level/BlockSource.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/Level.h"
#include "world/entity/player/Player.h"
#include "world/item/McItem.h"
#include "world/redstone/Redstone.h"
#include "world/Facing.h"

DispenserBlock::DispenserBlock(const std::string& nameId, int id)
	: EntityBlock(nameId, id, Material::getMaterial(MaterialType::Stone)) {
	mBlockEntityType = BlockEntityType::Dispenser;
}

int DispenserBlock::getVariant(int data) const {
	int dir = getBlockState(BlockState::FacingDirection).get<int>(data);

	if (dir == Facing::UP || dir == Facing::DOWN) {
		return 1;
	}
	else {
		return 0;
	}
}

FacingID DispenserBlock::getMappedFace(FacingID face, int data) const {
	int dir = getBlockState(BlockState::FacingDirection).get<int>(data);

	if (face == dir) {
		if (dir == Facing::UP || dir == Facing::DOWN) {
			return Facing::EAST;
		}
		else {
			return Facing::SOUTH;
		}
	}

	if (dir == Facing::UP || dir == Facing::DOWN) {
		return Facing::UP;
	}
	else if (face == Facing::UP || face == Facing::DOWN) {
		return Facing::UP;
	}
	return Facing::NORTH;
}

int DispenserBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	return getPlacementFacingAll(by, pos, 0.0f);
}

bool DispenserBlock::use(Player& player, const BlockPos& pos) const {
	return true;
}

bool DispenserBlock::isInteractiveBlock() const {
	return true;
}

int DispenserBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Block::mDispenser->mID;
}

DataID DispenserBlock::getSpawnResourcesAuxValue(DataID data) const {
	return Facing::SOUTH;
}

ItemInstance DispenserBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(Block::mDispenser, 1, Facing::SOUTH);
}

ItemInstance DispenserBlock::getSilkTouchItemInstance(DataID data) const {
	return ItemInstance(Block::mDispenser, 1, Facing::SOUTH);
}

void DispenserBlock::onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const {
	if (region.getLevel().isClientSide()) {
		return;
	}

	FullBlock fb = region.getBlockAndData(pos);
	int data = fb.data;

	bool signal = strength > Redstone::SIGNAL_NONE;
	bool isTriggered = getBlockState(BlockState::TriggeredBit).getBool(data);

	if (signal && !isTriggered) {
		getBlockState(BlockState::TriggeredBit).set(fb.data, true);
		region.setBlockAndDataNoUpdate(pos.x, pos.y, pos.z, fb);
	} else if (!signal && isTriggered) {
		getBlockState(BlockState::TriggeredBit).set(fb.data, false);
		region.setBlockAndData(pos, fb, UPDATE_NONE);
	}
}

bool DispenserBlock::hasComparatorSignal() const {
	return true;
}

int DispenserBlock::getTickDelay() const {
	return 4;
}

Vec3 DispenserBlock::getDispensePosition(BlockSource& region, const Vec3& pos) const {
	const FacingID facing = getFacing(region.getData(pos));

	const float originX = pos.x + 0.5f + 0.7f * Facing::getStepX(facing);
	const float originY = pos.y + 0.3f + 0.7f * Facing::getStepY(facing);
	const float originZ = pos.z + 0.5f + 0.7f * Facing::getStepZ(facing);

	return Vec3(originX, originY, originZ);
}

FacingID DispenserBlock::getFacing(int data) const {
	return Facing::DIRECTIONS[getBlockState(BlockState::FacingDirection).get<int>(data) % 6];
}

void DispenserBlock::recalcLockDir(BlockSource& region, const BlockPos& pos) {
	if (region.getLevel().isClientSide()) {
		return;
	}

	FullBlock fb = region.getBlockAndData(pos);

	bool solidBlockingNorth = region.isSolidBlockingBlock(pos.north());
	bool solidBlockingSouth = region.isSolidBlockingBlock(pos.south());
	bool solidBlockingEast = region.isSolidBlockingBlock(pos.east());
	bool solidBlockingWest = region.isSolidBlockingBlock(pos.west());

	int lockDir = fb.data;
	if (solidBlockingNorth && !solidBlockingSouth) {
		lockDir = 3;
	}
	if (solidBlockingSouth && !solidBlockingNorth) {
		lockDir = 2;
	}
	if (solidBlockingWest && !solidBlockingEast) {
		lockDir = 5;
	}
	if (solidBlockingEast && !solidBlockingWest) {
		lockDir = 4;
	}

	getBlockState(BlockState::FacingDirection).set(fb.data, lockDir);
	region.setBlockAndData(pos, fb, Block::UPDATE_CLIENTS);
}
