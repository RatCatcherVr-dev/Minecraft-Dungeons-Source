/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/HopperBlock.h"
#include "world/item/ItemInstance.h"
#include "world/level/material/Material.h"
#include "client/renderer/texture/TextureAtlas.h"
#include "world/item/McItem.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/Level.h"
#include "world/level/BlockSource.h"
#include "world/entity/player/Player.h"
#include "world/Facing.h"

int HopperBlock::getAttachedFace(int data) {
	int facingDir = Block::mHopper->getBlockState(BlockState::FacingDirection).get<int>((DataID)data);
	return facingDir;
}

bool HopperBlock::isTurnedOn(int data) {
	bool bToggleEnabled = Block::mHopper->getBlockState(BlockState::ToggleBit).getBool((DataID)data);
	return !bToggleEnabled;
}

HopperBlock::HopperBlock(const std::string& nameId, int id)
	: EntityBlock(nameId, id, Material::getMaterial(MaterialType::Metal)) {
	setVisualShape(Vec3::ZERO, Vec3::ONE);
	setSolid(false);
	mBlockEntityType = BlockEntityType::Hopper;
	mProperties = BlockProperty::Hopper | BlockProperty::TopSolidBlocking;
	mRenderLayer = BlockRenderLayer::RENDERLAYER_ALPHATEST_SINGLE_SIDE;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

void HopperBlock::onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const {
	FullBlock fb = region.getBlockAndData(pos);

	bool shouldBeOn = strength == 0;
	bool isOn = isTurnedOn(fb.data);

	if (shouldBeOn != isOn) {
		getBlockState(BlockState::ToggleBit).set(fb.data, shouldBeOn ? false : true);
		region.setBlockAndData(pos, fb, UPDATE_NONE);
	}
}

bool HopperBlock::use(Player& player, const BlockPos& pos) const {
	return true;
}

bool HopperBlock::isInteractiveBlock() const {
	return true;
}

bool HopperBlock::addCollisionShapes(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes, Entity* entity) const {
	addAABBs(region, pos, intersectTestBox, inoutBoxes);
	return true;
}

void HopperBlock::addAABBs(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes) const {
	AABB shape(Vec3::ZERO, Vec3(1, 10.0f / 16.0f, 1));
	Block::addAABB(shape.move(pos), intersectTestBox, inoutBoxes);
	float thickness = 2.0f / 16.0f;
	shape.set(Vec3::ZERO, Vec3(thickness, 1, 1));
	Block::addAABB(shape.move(pos), intersectTestBox, inoutBoxes);
	shape.set(Vec3::ZERO, Vec3(1, 1, thickness));
	Block::addAABB(shape.move(pos), intersectTestBox, inoutBoxes);
	shape.set(Vec3(1 - thickness, 0, 0), Vec3::ONE);
	Block::addAABB(shape.move(pos), intersectTestBox, inoutBoxes);
	shape.set(Vec3(0, 0, 1 - thickness), Vec3::ONE);
	Block::addAABB(shape.move(pos), intersectTestBox, inoutBoxes);
}

bool HopperBlock::canProvideSupport(BlockSource& region, const BlockPos& pos, FacingID face, BlockSupportType type) const {
	return face == Facing::UP;
}

int HopperBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	int attached = Facing::OPPOSITE_FACING[face];
	if (attached == Facing::UP) {
		attached = Facing::DOWN;
	}

	DataID data = 0;
	getBlockState(BlockState::FacingDirection).set(data, attached);

	return data;
}

bool HopperBlock::hasComparatorSignal() const {
	return true;
}

int HopperBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return McItem::mHopper->getId();
}

ItemInstance HopperBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(McItem::mHopper, 1, blockData);
}

bool HopperBlock::canBeSilkTouched() const {
	return false;
}
