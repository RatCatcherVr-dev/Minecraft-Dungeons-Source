/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/SkullBlock.h"
#include "world/level/material/Material.h"
#include "world/Facing.h"
#include "world/level/Level.h"
#include "world/entity/player/Player.h"
#include "world/level/BlockSource.h"
#include "world/item/McItem.h"
#include "world/level/dimension/Dimension.h"

SkullBlock::SkullBlock(const std::string& nameId, int id)
	: EntityBlock(nameId, id, Material::getMaterial(MaterialType::Decoration)) {
	setVisualShape(Vec3(4.0f / 16.0f, 0, 4.0f / 16.0f), Vec3(12.0f / 16.0f, .5f, 12.0f / 16.0f));

	setSolid(false);
	mBlockEntityType = BlockEntityType::Skull;
	mProperties = BlockProperty::BreakOnPush;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

bool SkullBlock::mayPlace(BlockSource& region, const BlockPos& pos) const {
	return EntityBlock::mayPlace(region, pos);
}

std::string SkullBlock::getTypeDescriptionId(int data) {
	static const std::array<std::string, 6> SKULL_NAMES = {
		{"skeleton", "wither", "zombie", "char", "creeper", "dragon"} //D11.PS - Switch needed braces
	};

	int type = Block::mSkull->getBlockState(BlockState::MappedType).get<int>(data);

	if(type < 0 || type >= (int)SKULL_NAMES.size()) {
		type = 0;
	}

	return SKULL_NAMES[type];
}

ItemInstance SkullBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	SkullBlockEntity* entity = (SkullBlockEntity*)region.getBlockEntity(pos);
// 	return ItemInstance(McItem::mSkull->getId(), 1, enum_cast(entity->getSkullType()));
	return ItemInstance(McItem::mSkull->getId(), 1, 0);
}

const AABB& SkullBlock::getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping) const {
	int dir = getBlockState(BlockState::FacingDirection).get<int>(region.getData(pos));

	switch (dir) {
	default:
	case Facing::UP:
		bufferAABB.set(4.0f / 16.0f, 0, 4.0f / 16.0f, 12.0f / 16.0f, .5f, 12.0f / 16.0f);
		break;
	case Facing::NORTH:
		bufferAABB.set(4.0f / 16.0f, 4.0f / 16.0f, .5f, 12.0f / 16.0f, 12.0f / 16.0f, 1);
		break;
	case Facing::SOUTH:
		bufferAABB.set(4.0f / 16.0f, 4.0f / 16.0f, 0, 12.0f / 16.0f, 12.0f / 16.0f, .5f);
		break;
	case Facing::WEST:
		bufferAABB.set(.5f, 4.0f / 16.0f, 4.0f / 16.0f, 1, 12.0f / 16.0f, 12.0f / 16.0f);
		break;
	case Facing::EAST:
		bufferAABB.set(0, 4.0f / 16.0f, 4.0f / 16.0f, .5f, 12.0f / 16.0f, 12.0f / 16.0f);
		break;
	}

	return bufferAABB;
}

bool SkullBlock::canBeSilkTouched() const {
	return false;
}

DataID SkullBlock::getSpawnResourcesAuxValue(DataID data) const {
	return data;
}

bool SkullBlock::_witherSkullTester(BlockSource& region, const BlockPos& pos, BlockID id) const {
	return false;
}

void SkullBlock::spawnResources(BlockSource& region, const BlockPos& pos, int data, float odds, int bonusLootLevel) const {
	// do nothing, resource is popped by onRemove
	// ... because the block entity is removed prior to spawnResources
}

int SkullBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return McItem::mSkull->getId();
}

void SkullBlock::onRemove(BlockSource& region, const BlockPos& pos) const {
	if (region.getLevel().isClientSide()) {
		return;
	}

	FullBlock t = region.getBlockAndData(pos);
	if(!getBlockState(BlockState::NoDropBit).getBool(t.data)) {
		ItemInstance item(McItem::mSkull->getId(), 1, 0);
		popResource(region, pos, item);
	}
	EntityBlock::onRemove(region, pos);
}

void SkullBlock::checkMobSpawn(Level& level, BlockSource& region, const BlockPos& pos, SkullBlockEntity& placedSkull) const {
}

std::string SkullBlock::buildDescriptionName(DataID data) const {
	// descriptionId is tile.*, must replace with item for localization lookup
	std::string itemId = mDescriptionId;
	itemId.replace(0, 4, "item");

	return I18n::get(itemId + "." + getTypeDescriptionId(data) + ".name");
}

int SkullBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	DataID dir = (Math::floor(by.getRotation().y * 4 / (360) + 2.5f)) & 3;
	bool bNoDrop = getBlockState(BlockState::NoDropBit).getBool(itemValue);

	DataID data = 0;
	getBlockState(BlockState::NoDropBit).set(data, bNoDrop);

	if (dir == 0) {
		getBlockState(BlockState::FacingDirection).set(data, Facing::NORTH);
	} else if (dir == 1) {
		getBlockState(BlockState::FacingDirection).set(data, Facing::EAST);
	} else if (dir == 2) {
		getBlockState(BlockState::FacingDirection).set(data, Facing::SOUTH);
	} else if (dir == 3) {
		getBlockState(BlockState::FacingDirection).set(data, Facing::WEST);
	}

	return data;
}
