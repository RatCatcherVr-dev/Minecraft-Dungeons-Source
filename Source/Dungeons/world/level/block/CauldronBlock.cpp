/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/CauldronBlock.h"
#include "world/level/material/Material.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/entity/Entity.h"
#include "world/entity/player/Player.h"
#include "world/Facing.h"
#include "world/item/McItem.h"
#include "world/item/ItemInstance.h"

int CauldronBlock::MIN_FILL_LEVEL = 0;
int CauldronBlock::MAX_FILL_LEVEL = 6;
int CauldronBlock::BASE_WATER_PIXEL = 5;
int CauldronBlock::PIXEL_PER_LEVEL = 2;

CauldronBlock::CauldronBlock(const std::string& nameId, int id)
	: EntityBlock(nameId, id, Material::getMaterial(MaterialType::Metal)) {
	mRenderLayer = RENDERLAYER_OPAQUE;
	setSolid(false);
	setTicking(false);
	mProperties = BlockProperty::Unspecified;

	mAnimatedTexture = true;
	mBlockEntityType = BlockEntityType::Cauldron;

	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

bool CauldronBlock::addCollisionShapes(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes, Entity* entity) const {
	addAABBs(region, pos, intersectTestBox, inoutBoxes);
	return true;
}

void CauldronBlock::addAABBs(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes) const {
	AABB shape(Vec3::ZERO, Vec3(1, 5.0f / 16.0f, 1));
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

void CauldronBlock::updateShape(BlockSource& region, const BlockPos& pos) {
	setVisualShape(Vec3::ZERO, Vec3::ONE);
}

bool CauldronBlock::canProvideSupport(BlockSource& region, const BlockPos& pos, FacingID face, BlockSupportType type) const {
	return face == Facing::UP && (type == BlockSupportType::Any || type == BlockSupportType::Edge);
}

void CauldronBlock::handleEntityInside(BlockSource& region, const BlockPos& pos, Entity* entity, Vec3& current) const {
	int fillLevel = getBlockState(BlockState::FillLevel).get<int>(region.getData(pos));
	float waterLevel = pos.y + ((float)BASE_WATER_PIXEL + PIXEL_PER_LEVEL * fillLevel) / 16.0f;

	if (!region.getLevel().isClientSide() && entity->isOnFire() && fillLevel > 0 && entity->mBB.min.y <= waterLevel) {
		current.y = 0.1f;
		setWaterLevel(region, pos, fillLevel, fillLevel - 1, entity);
	}
}

bool CauldronBlock::use(Player& player, const BlockPos& pos) const {
	return true;
}

void CauldronBlock::setWaterLevel(BlockSource& region, const BlockPos& pos, DataID data, int waterLevel, Entity* setter) const {
	getBlockState(BlockState::FillLevel).set(data, Math::clamp(waterLevel, MIN_FILL_LEVEL, MAX_FILL_LEVEL));
	region.setBlockAndData(pos, {mID, data}, Block::UPDATE_ALL, setter);
}

int CauldronBlock::getWaterLevel(DataID data) {
	int waterLevel = data;
	return Math::clamp(waterLevel, MIN_FILL_LEVEL, MAX_FILL_LEVEL);
}

void CauldronBlock::handleRain(BlockSource& region, const BlockPos& pos, float downfallAmount) const {
	if (region.getLevel().getRandom().nextFloat() * 0.5f > downfallAmount) {
		return;
	}

	DataID data = region.getData(pos);
	int fillLevel = data;

	if (data < MAX_FILL_LEVEL) {
		region.setBlockAndData(pos, FullBlock(mID, data + 1), Block::UPDATE_ALL);
	}
}

int CauldronBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return McItem::mCauldron->getId();
}

ItemInstance CauldronBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(McItem::mCauldron, 1);
}

bool CauldronBlock::hasComparatorSignal() const {
	return true;
}

int CauldronBlock::getComparatorSignal(BlockSource& region, const BlockPos& pos, FacingID dir, int data) const {
	int fillLevel = getBlockState(BlockState::FillLevel).get<int>(data);
	return (fillLevel + 1) / 2;
}

ItemInstance CauldronBlock::getSilkTouchItemInstance(DataID data) const {
	return ItemInstance(McItem::mCauldron);
}

void CauldronBlock::_decrementStack(Player& player, ItemInstance* current) const {
	current->remove(1);
}

void CauldronBlock::_explodeCauldronContents(BlockSource& region, const BlockPos& pos, DataID data) const {
	setWaterLevel(region, pos, data, MIN_FILL_LEVEL, nullptr);
}

int CauldronBlock::getExtraRenderLayers() const {
	return (1 << RENDERLAYER_BLEND);
}
