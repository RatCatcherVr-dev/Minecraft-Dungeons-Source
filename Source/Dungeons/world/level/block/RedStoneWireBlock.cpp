/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/RedStoneWireBlock.h"
#include "world/Direction.h"
#include "world/level/BlockSource.h"
#include "world/level/material/Material.h"
#include "world/level/Level.h"
#include "world/level/dimension/Dimension.h"
#include "world/item/McItem.h"
#include "world/item/ItemInstance.h"
#include "PistonBlock.h"

RedStoneWireBlock::RedStoneWireBlock(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::RedstoneWire)) {
	setVisualShape(Vec3::ZERO, Vec3(1, 1 / 16.0f, 1));
	setSolid(false);
	mProperties = BlockProperty::BreakOnPush;

	mRenderLayer = RENDERLAYER_ALPHATEST;
	mThickness = 0.07f;

	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

const AABB& RedStoneWireBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	if (isClipping) {
		bufferValue = getVisualShape(region, pos, bufferValue, true);
		return bufferValue.move(pos);
	}
	return AABB::EMPTY;
}

bool RedStoneWireBlock::canSurvive(BlockSource& region, const BlockPos& pos) const {
	auto& blockRef = region.getBlock(pos.below());
	bool haveSupport = region.canProvideSupport(pos.below(), Facing::UP, BlockSupportType::Center) && region.canProvideSupport(pos.below(), Facing::UP, BlockSupportType::Edge);
	return haveSupport || blockRef.isType(Block::mGlowStone) ||
		blockRef.isType(Block::mUnlitRedStoneLamp) || blockRef.isType(Block::mLitRedStoneLamp);
}

bool RedStoneWireBlock::mayPlace(BlockSource& region, const BlockPos& pos) const {
	return canSurvive(region, pos);
}

void RedStoneWireBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	Block::onPlace(region, pos);
	onLoaded(region, pos);
}

void RedStoneWireBlock::onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const {
	if (region.getLevel().isClientSide() == false) {
		FullBlock fullBlock = region.getBlockAndData(pos);
		region.setBlockAndData(pos, fullBlock.id, strength, Block::UPDATE_ALL);
	}
}

void RedStoneWireBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	if (!region.getLevel().isClientSide() && !canSurvive(region, pos)) {
		spawnResources(region, pos, 0, 1);
		region.removeBlock(pos);
	}
}

int RedStoneWireBlock::getColor(BlockSource& region, const BlockPos& pos, DataID auxData) const {
	return getColor(auxData);
}

int RedStoneWireBlock::getColor(int data) const {
	int signal = getBlockState(BlockState::RedstoneSignal).get<int>(data);
	Color color;

	float pow = (signal / 15.0f);
	color.r = pow * 0.6f + 0.4f;
	if (signal == 0) {
		color.r = 0.3f;
	}

	color.g = pow * pow * 0.7f - 0.5f;
	color.b = pow * pow * 0.6f - 0.7f;
	if (color.g < 0) {
		color.g = 0;
	}
	if (color.b < 0) {
		color.b = 0;
	}

	return color.toARGB();
}


int RedStoneWireBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return McItem::mRedStone->getId();
}

ItemInstance RedStoneWireBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(McItem::mRedStone);
}

ItemInstance RedStoneWireBlock::getSilkTouchItemInstance(DataID data) const {
	return ItemInstance(McItem::mRedStone);
}

bool RedStoneWireBlock::shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const {
	return true;
}

bool RedStoneWireBlock::shouldConnectTo(BlockSource &region, const BlockPos& pos, int direction) {
	const Block& block = region.getBlock(pos);
	return block.shouldConnectToRedstone(region, pos, direction);
	// DG:TODO remove function and just call shouldConnectToRedstone
}

