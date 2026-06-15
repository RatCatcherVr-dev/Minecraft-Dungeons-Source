/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/BorderBlock.h"
#include "world/level/Level.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/material/Material.h"
#include "world/level/BlockSource.h"
#include "world/entity/player/Player.h"
#include "world/item/McItem.h"
#include "client/renderer/texture/TextureAtlasItem.h"
#include "world/level/BlockSource.h"

BorderBlock::BorderBlock(const std::string& nameId, int id)
	: WallBlock(nameId, id, Material::getMaterial(MaterialType::Stone)) {
	setDestroyTime(0.2f);
	setVisualShape(Vec3(0, 0, 0), Vec3(1, 1, 1));

	mProperties = BlockProperty::Immovable;
}

std::string BorderBlock::buildDescriptionName(DataID data) const {
	return I18n::get("tile.border_block.name");
}

bool BorderBlock::connectsTo(BlockSource& region, const BlockPos& thisPos, const BlockPos& otherPos) const {
	BlockID otherBlockId = region.getBlockID(otherPos);
	if (otherBlockId == mID) {
		return true;
	}
	return false;
}

const AABB& BorderBlock::getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping) const {
	return Block::getVisualShape(region, pos, bufferAABB, isClipping);
}

int BorderBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const{
	return 1;
}

void BorderBlock::onLoaded(BlockSource& region, const BlockPos& pos) const {
	//Only happens on server side
	if (!region.getLevel().isClientSide()) {
		//add to level chunks mBorderBlocks
		region.setBorderBlock(pos, true);
	}
}

bool BorderBlock::use(Player& player, const BlockPos& pos) const {
	return Block::use(player, pos);
}

void BorderBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	Block::onPlace(region, pos);
	onLoaded(region, pos);
}

void BorderBlock::onRemove(BlockSource& region, const BlockPos& pos) const {
	region.setBorderBlock(pos, false);
}
