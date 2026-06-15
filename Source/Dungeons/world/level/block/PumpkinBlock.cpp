/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/PumpkinBlock.h"
#include "world/level/material/Material.h"
#include "world/Facing.h"
#include "world/level/BlockSource.h"
#include "world/entity/Entity.h"
#include "world/item/ItemInstance.h"

PumpkinBlock::PumpkinBlock(const std::string& nameId, int id, bool lit)
	: Block(nameId, id, Material::getMaterial(MaterialType::Vegetable))
	, mLit(lit) {
	mProperties = BlockProperty::CubeShaped | BlockProperty::BreakOnPush;
}

int PumpkinBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	int dir = Math::floor(by.getRotation().y * 4 / (360) + 2.5f) & 3;

	DataID data = 0;
	getBlockState(BlockState::Direction).set(data, dir);
	
	return data;
}

ItemInstance PumpkinBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(mLit ? Block::mLitPumpkin : Block::mPumpkin);
}

bool PumpkinBlock::_golemPumpkinTester(BlockSource& region, const BlockPos& pos, BlockID id) const {
	return id == Block::mPumpkin->mID || id == Block::mLitPumpkin->mID;
}

bool PumpkinBlock::_canSpawnGolem(BlockSource& region, const BlockPos& pos) const {
	return false;
}

int PumpkinBlock::getVariant(int data) const {
	return mLit ? 1 : 0;
}

FacingID PumpkinBlock::getMappedFace(FacingID face, int data) const {
	int dir = getBlockState(BlockState::Direction).get<int>(data);

	switch (face) {
	case Facing::UP:
		return Facing::UP;
	case Facing::DOWN:
		return Facing::UP;
	case Facing::NORTH:
		if (dir == DIR_NORTH) {
			return Facing::SOUTH;
		}
		break;
	case Facing::EAST:
		if (dir == DIR_EAST) {
			return Facing::SOUTH;
		}
		break;
	case Facing::SOUTH:
		if (dir == DIR_SOUTH) {
			return Facing::SOUTH;
		}
		break;
	case Facing::WEST:
		if (dir == DIR_WEST) {
			return Facing::SOUTH;
		}
		break;
	}

	return Facing::NORTH;
}

void PumpkinBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	Block::onPlace(region, pos);

	_canSpawnGolem(region, pos);
}

bool PumpkinBlock::mayPlace(BlockSource& region, const BlockPos& pos) const {
	BlockID t = region.getBlockID(pos);
	return (t == 0 || Block::mBlocks[t]->getMaterial().isReplaceable()) && region.canProvideSupport(pos.below(), Facing::UP, BlockSupportType::Any); // TODO: It should not care about canProvideSupport!
}
