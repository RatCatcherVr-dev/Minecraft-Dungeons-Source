/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/RedstoneLampBlock.h"
#include "world/item/ItemInstance.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/material/Material.h"
#include "world/redstone/Redstone.h"

RedstoneLampBlock::RedstoneLampBlock(const std::string& nameId, int id, bool isLit)
	: Block(nameId, id, Material::getMaterial(MaterialType::BuildableGlass))
	, mIsLit(isLit) {

	if (mIsLit) {
		setLightEmission(1.0f);
	}
}

int RedstoneLampBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Block::mUnlitRedStoneLamp->mID;
}

ItemInstance RedstoneLampBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(Block::mUnlitRedStoneLamp);
}

ItemInstance RedstoneLampBlock::getSilkTouchItemInstance(DataID data) const {
	return ItemInstance(mUnlitRedStoneLamp);
}

bool RedstoneLampBlock::shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const {
	return false;
}

void RedstoneLampBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	Block::onPlace(region, pos);
	onLoaded(region, pos);
}
