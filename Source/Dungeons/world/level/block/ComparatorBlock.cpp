/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/ComparatorBlock.h"
#include "world/level/block/entity/BlockEntity.h"
#include "world/item/McItem.h"
#include "world/level/BlockSource.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/material/Material.h"
#include "world/level/Level.h"
#include "world/entity/Mob.h"
#include "world/entity/player/Player.h"
#include "world/Direction.h"
#include "world/redstone/Redstone.h"
#include "world/Facing.h"
#include "world/item/ItemInstance.h"

ComparatorBlock::ComparatorBlock(const std::string& nameId, int id, bool on)
	: EntityBlock(nameId, id, Material::getMaterial(MaterialType::Decoration)) {
	mBlockEntityType = BlockEntityType::Comparator;
	setVisualShape(Vec3::ZERO, Vec3(1, 2.0f / 16.0f, 1));
	mRenderLayer = BlockRenderLayer::RENDERLAYER_ALPHATEST_SINGLE_SIDE;
	mProperties = BlockProperty::BreakOnPush;
	setSolid(false);
	mOn = on;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

int ComparatorBlock::getVariant(int data) const {
	return mOn ? 1 : 0;
}

bool ComparatorBlock::shouldRenderFace(BlockSource& region, const BlockPos& pos, FacingID face, const AABB& shape) const {
	if (face == Facing::DOWN || face == Facing::UP) {
		// up and down is a special case handled by the shape renderer
		return false;
	}
	return true;
}

bool ComparatorBlock::use(Player& player, const BlockPos& pos) const {
	BlockSource& region = player.getRegion();

	DataID data = region.getData(pos);
	bool subtractBit = getBlockState(BlockState::OutputSubtractBit).getBool(data);
	bool litBit = getBlockState(BlockState::OutputLitBit).getBool(data);

	getBlockState(BlockState::OutputSubtractBit).set(data, !subtractBit);

	region.setBlockAndData(pos, {mID, data}, UPDATE_ALL, &player);

	return true;
}

bool ComparatorBlock::isInteractiveBlock() const {
	return true;
}

bool ComparatorBlock::mayPlace(BlockSource& region, const BlockPos& pos) const {
	if (!region.canProvideSupport(pos.below(), Facing::UP, BlockSupportType::Any)) {
		return false;
	}
	return EntityBlock::mayPlace(region, pos);
}

bool ComparatorBlock::canSurvive(BlockSource& region, const BlockPos& pos) const {
	if (!region.canProvideSupport(pos.below(), Facing::UP, BlockSupportType::Any)) {
		return false;
	}
	return EntityBlock::canSurvive(region, pos);
}

int ComparatorBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	return (((Math::floor(by.mRot.y * 4 / (360) + 0.5f)) & 3) + 2) % 4;
}

void ComparatorBlock::triggerEvent(BlockSource& region, const BlockPos& pos, int b0, int b1) const {
	EntityBlock::triggerEvent(region, pos, b0, b1);
	BlockEntity *be = region.getBlockEntity(pos);
	if (be != nullptr) {
		return be->triggerEvent(b0, b1);
	}
}

int ComparatorBlock::getDirectSignal(BlockSource& region, const BlockPos& pos, int dir) const {
	return getSignal(region, pos, dir);
}

bool ComparatorBlock::shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const
{
	return true;
}

int ComparatorBlock::getSignal(BlockSource& region, const BlockPos& pos, int dir) const {

	int dirFacing = Direction::DIRECTION_FACING[getBlockState(BlockState::Direction).get<int>(region.getData(pos))];
	if (dir != dirFacing) {
		return Redstone::SIGNAL_NONE;
	}
	return (mOn ? Redstone::SIGNAL_MAX : Redstone::SIGNAL_NONE);
}

bool ComparatorBlock::isSignalSource() const {
	return true;
}

void ComparatorBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	if (!canSurvive(region, pos)) {
		popResource(region, pos, ItemInstance(McItem::mComparator));
		region.removeBlock(pos);
	}
}

int ComparatorBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return McItem::mComparator->getId();
}

ItemInstance ComparatorBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(McItem::mComparator);
}

bool ComparatorBlock::canBeSilkTouched() const {
	return false;
}

bool ComparatorBlock::isSubtractMode(BlockSource& region, const BlockPos& pos) const {
	return getBlockState(BlockState::OutputSubtractBit).getBool(region.getData(pos));
}
