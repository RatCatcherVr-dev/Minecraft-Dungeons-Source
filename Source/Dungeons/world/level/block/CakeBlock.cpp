/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "CakeBlock.h"
#include "world/entity/player/Player.h"
#include "world/level/material/Material.h"
// #include "world/food/FoodConstants.h"
#include "world/level/BlockSource.h"
#include "world/Facing.h"
#include "world/level/Level.h"

CakeBlock::CakeBlock(const std::string& nameId, int id) :
	Block(nameId, id, Material::getMaterial(MaterialType::Cake)) {

	mProperties = BlockProperty::BreakOnPush;

	setSolid(false);
	setPushesOutItems(true);
}

bool CakeBlock::use(Player& player, const BlockPos& pos) const {
	return true;
}

bool CakeBlock::mayPlace(BlockSource& region, const BlockPos& pos, FacingID face) const {
	if(Block::mayPlace(region, pos, face)) {
		return canSurvive(region, pos);
	}

	return false;
}

const AABB& CakeBlock::getVisualShape(DataID data, AABB& bufferAABB, bool isClipping) const {
	int bites = getBlockState(BlockState::BiteCounter).get<int>(data);
	float r = 1 / 16.0f;
	float r2 = (1 + bites * 2) / 16.0f;
	float h = 8 / 16.0f;
	bufferAABB.set(r2, 0, r, 1 - r, h, 1 - r);
	return bufferAABB;
}

bool CakeBlock::canSurvive(BlockSource& region, const BlockPos& pos) const {
	return region.getMaterial(pos.below()).isSolid();
}

void CakeBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	if (!canSurvive(region, pos)) {
		//TODO the particles should really happen on removeBlock() on the client
		region.removeBlock(pos);
	}
}

int CakeBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 0;
}

bool CakeBlock::hasComparatorSignal() const {
	return true;
}

int CakeBlock::getComparatorSignal(BlockSource& region, const BlockPos& pos, FacingID dir, int data) const {
	int bites = getBlockState(BlockState::BiteCounter).get<int>(region.getData(pos));
	return (1 + MAX_BITES - bites) * 2;
}

bool CakeBlock::canBeSilkTouched() const {
	return false;
}
