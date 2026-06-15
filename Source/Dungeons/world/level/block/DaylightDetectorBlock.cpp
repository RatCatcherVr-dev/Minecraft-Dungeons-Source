/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/DaylightDetectorBlock.h"

#include "util/Math.h"
#include "world/Facing.h"
#include "world/level/BlockSource.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/material/Material.h"
#include "world/redstone/Redstone.h"
#include "world/level/Level.h"
#include "world/entity/player/Player.h"

DaylightDetectorBlock::DaylightDetectorBlock(const std::string& nameId, int id, bool isInverted)
	: EntityBlock(nameId, id, Material::getMaterial(MaterialType::Wood))
	, mIsInverted(isInverted)
{
	setVisualShape(Vec3(0, 0, 0), Vec3(1, 6.0f / 16.0f, 1));

	setSolid(false);
	setPushesOutItems(true);

	mProperties = BlockProperty::Unspecified;
    mBlockEntityType = BlockEntityType::DaylightDetector;
}

void DaylightDetectorBlock::updateShape(BlockSource& level, const BlockPos& pos) {
	setVisualShape(Vec3(0, 0, 0), Vec3(1, 6.0f / 16.0f, 1));
}

void DaylightDetectorBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	onLoaded(region, pos);
}


bool DaylightDetectorBlock::use(Player& player, const BlockPos& pos) const {
	return true;
}

bool DaylightDetectorBlock::shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const {
	return true;
}

int DaylightDetectorBlock::getVariant(int data) const {
	return mIsInverted ? 1 : 0;
}

int DaylightDetectorBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return mDaylightDetector->mID;
}

ItemInstance DaylightDetectorBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(mDaylightDetector);
}

ItemInstance DaylightDetectorBlock::getSilkTouchItemInstance(DataID data) const {
	return ItemInstance(mDaylightDetector);
}
