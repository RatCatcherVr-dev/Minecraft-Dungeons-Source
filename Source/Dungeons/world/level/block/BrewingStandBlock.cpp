/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/BrewingStandBlock.h"
#include "world/level/material/Material.h"
#include "world/entity/player/Player.h"
#include "world/item/ItemInstance.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/item/McItem.h"

bool BrewingStandBlock::mNoDrop = false;

BrewingStandBlock::BrewingStandBlock(const std::string& nameId, int id)
	: EntityBlock(nameId, id, Material::getMaterial(MaterialType::Metal)) {
	setSolid(false);
	setPushesOutItems(true);
	mProperties = BlockProperty::Unspecified;
	mRenderLayer = BlockRenderLayer::RENDERLAYER_ALPHATEST;
	mBlockEntityType = BlockEntityType::BrewingStand;
	setVisualShape(Vec3::ZERO, Vec3::ONE);
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

int BrewingStandBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return McItem::mBrewing_stand->getId();
}

ItemInstance BrewingStandBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(McItem::mBrewing_stand);
}

std::string BrewingStandBlock::buildDescriptionName(DataID data) const {
	return I18n::get("item.brewing_stand.name");
}

void BrewingStandBlock::addAABBs(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes) const {
	AABB shape(Vec3(7.0f / 16.0f, 0, 7.0f / 16.0f), Vec3(9.0f / 16.0f, 14.0f / 16.0f, 9.0f / 16.0f));
	Block::addAABB(shape.move(pos), intersectTestBox, inoutBoxes);
	shape.set(Vec3::ZERO, Vec3(1, 2.0f / 16.0f, 1));
	Block::addAABB(shape.move(pos), intersectTestBox, inoutBoxes);
}

bool BrewingStandBlock::addCollisionShapes( BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes, Entity* entity ) const {
	addAABBs(region, pos, intersectTestBox, inoutBoxes);
	return true;
}

bool BrewingStandBlock::use(Player& player, const BlockPos& pos) const {
	return true;
}
bool BrewingStandBlock::isContainerBlock() const {
	return true;
}

bool BrewingStandBlock::isCraftingBlock() const {
	return true;
}

bool BrewingStandBlock::hasComparatorSignal() const {
	return true;
}

bool BrewingStandBlock::canBeSilkTouched() const {
	return false;
}
