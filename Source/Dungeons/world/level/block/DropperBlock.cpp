/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/DropperBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/level/material/Material.h"
#include "world/item/ItemInstance.h"
#include "world/item/McItem.h"
#include "world/Facing.h"

DropperBlock::DropperBlock(const std::string& nameId, int id)
	: DispenserBlock(nameId, id) {
	mBlockEntityType = BlockEntityType::Dropper;
}

int DropperBlock::getAttachedFace(int data) {
	return Facing::DIRECTIONS[Block::mDropper->getBlockState(BlockState::FacingDirection).get<int>(data) % 6];
}

int DropperBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Block::mDropper->mID;
}

ItemInstance DropperBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(Block::mDropper, 1, Facing::SOUTH);
}

ItemInstance DropperBlock::getSilkTouchItemInstance(DataID data) const {
	return ItemInstance(Block::mDropper, 1, Facing::SOUTH);
}
