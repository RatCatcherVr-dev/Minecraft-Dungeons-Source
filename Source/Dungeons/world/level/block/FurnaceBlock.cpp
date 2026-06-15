/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/FurnaceBlock.h"
#include "world/level/block/entity/BlockEntity.h"
#include "world/level/material/Material.h"
#include "world/Facing.h"
#include "world/entity/Mob.h"
#include "world/entity/player/Player.h"
#include "world/item/ItemInstance.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/item/McItem.h"

bool FurnaceBlock::mNoDrop = false;

FurnaceBlock::FurnaceBlock(const std::string& nameId, int id, bool lit)
	: EntityBlock(nameId, id, Material::getMaterial(MaterialType::Stone))
	, mLit(lit){

	mBlockEntityType = BlockEntityType::Furnace;
}

int FurnaceBlock::getResource(Random& random, int data, int bonusLootLevel) const{
	return Block::mFurnace->mID;
}

FacingID FurnaceBlock::getMappedFace(FacingID face, int data) const {
	if (face == Facing::UP || face == Facing::DOWN) {
		return Facing::UP;
	}

	int dir = getBlockState(BlockState::FacingDirection).get<int>(data);

	if (face != dir) {
		return Facing::NORTH;
	}

	if (mLit) {
		return Facing::EAST;
	}
	else {
		return Facing::SOUTH;
	}
}


bool FurnaceBlock::use(Player& player, const BlockPos& pos) const {
	//TODO remove when creative inventories are added
	if (player.getLevel().isClientSide()) {
		return true;
	}

// 	BlockEntity* blockEntity = player.getRegion().getBlockEntity(pos);
// 	if (blockEntity != nullptr && blockEntity->isType(BlockEntityType::Furnace)) {
// 		player.openFurnace(static_cast<FurnaceBlockEntity*>(blockEntity)->getContainerSize(), blockEntity->getPosition());
// 	}

	return true;
}

void FurnaceBlock::setLit(bool lit, BlockSource& region, const BlockPos& pos){
	FullBlock block = region.getBlockAndData(pos);
	BlockEntity* blockEntity = region.getBlockEntity(pos);

	if (blockEntity != nullptr && blockEntity->isType(BlockEntityType::Furnace)) {
		mNoDrop = true;

		block.id = lit ? Block::mLitFurnace->mID : Block::mFurnace->mID;
		region.setBlockAndData(pos, block, Block::UPDATE_ALL);

		mNoDrop = false;
	}
}

int FurnaceBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const{
	DataID dir = (Math::floor(by.getRotation().y * 4 / (360) + 0.5f)) & 3;

	DataID data = 0;
	if (dir == 0) {
		getBlockState(BlockState::FacingDirection).set(data, Facing::NORTH);
	} else if (dir == 1) {
		getBlockState(BlockState::FacingDirection).set(data, Facing::EAST);
	} else if (dir == 2) {
		getBlockState(BlockState::FacingDirection).set(data, Facing::SOUTH);
	} else if (dir == 3) {
		getBlockState(BlockState::FacingDirection).set(data, Facing::WEST);
	}

	return data;
}

void FurnaceBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	// Confirm the data is in the correct range on first placement
	FullBlock fb = region.getBlockAndData(pos);

	// NOTE: This is written in a way that when created with commands and given a data value outside
	//	     the range [2, 5], the value will be looped around to a desired number within that range

	// Subtract 2 to get in a range starting from 0
	int newData = fb.data - 2;
	if (newData < 0) {
		newData += 15;
	}

	// Force the data to be in a range of [2, 5]
	fb.data = (newData % 4) + 2;
	region.setBlockAndData(pos, fb, Block::UPDATE_ALL);
}

bool FurnaceBlock::isContainerBlock() const {
	return true;
}

bool FurnaceBlock::isCraftingBlock() const {
	return true;
}

bool FurnaceBlock::hasComparatorSignal() const {
	return true;
}
