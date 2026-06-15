/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/ChestBlock.h"
#include "world/level/block/entity/BlockEntity.h"
#include "world/level/Level.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/material/Material.h"
#include "world/Facing.h"
#include "world/entity/player/Player.h"
#include "world/level/BlockSource.h"
#include "world/item/McItem.h"
#include "world/redstone/Redstone.h"

ChestBlock::ChestBlock(const std::string& nameId, int id, ChestBlock::ChestType type, MaterialType materialType)
	: EntityBlock(nameId, id, Material::getMaterial(materialType))
	, mType(type) {

	const float m = 0.025f;
	setVisualShape( Vec3(m, 0, m), Vec3(1 - m, 1 - m - m, 1 - m));

	setSolid(false);
	setPushesOutItems(true);

	mProperties = BlockProperty::Unspecified;
	mBlockEntityType = BlockEntityType::Chest;
}

Block& ChestBlock::init() {
	mTranslucency[mID] = 0.5f;
	return *this;
}

int ChestBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	int dir = (Math::floor(by.getRotation().y * 4 / (360) + 0.5f)) & 3;

	DataID data = 0;

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

bool ChestBlock::hasComparatorSignal() const {
	return true;
}

FacingID ChestBlock::getMappedFace(FacingID face, int data) const {
	if (face == Facing::UP || face == Facing::DOWN) {
		return Facing::UP;
	}

	int facing = getBlockState(BlockState::FacingDirection).get<int>(data);
	return (face == facing) ? Facing::SOUTH : Facing::EAST;
}

void ChestBlock::onLoaded(BlockSource& region, const BlockPos& pos) const {
	auto chest = region.getBlockAndData(pos);
	int facing = chest.data;
	if (facing != Facing::SOUTH && facing != Facing::WEST && facing != Facing::NORTH && facing != Facing::EAST) {
		// Chest data contains the facing: If we have an invalid facing, set to default (2 = Facing::NORTH)
		chest.data = Facing::NORTH;
		region.setBlockAndData(pos, chest, Block::UPDATE_ALL);
	}
}

bool ChestBlock::shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const {
	return (mType == TYPE_TRAP);
}

void ChestBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	EntityBlock::onPlace(region, pos);
	onLoaded(region, pos);
}

ChestBlockEntity* _teIfExisting(BlockSource& region, const BlockPos& pos) {
	return (ChestBlockEntity*)region.getBlockEntity(pos);
}

ChestBlockEntity& _te(BlockSource& region, const BlockPos& pos) {
	auto* chest = _teIfExisting(region, pos);
	DEBUG_ASSERT(chest, "Chest missing?");
	return *chest;
}

bool ChestBlock::use(Player& player, const BlockPos& pos) const {
	auto* chest = _teIfExisting(player.getRegion(), pos);
	if (!chest) {
		UE_LOG(LogDungeons, Warning, TEXT("chest tile without entity!!!"));
		return false;
	}
	return true;
}

bool ChestBlock::getSecondPart(BlockSource& region, const BlockPos& pos, BlockPos& out) const {
	return false;
}

bool ChestBlock::isContainerBlock() const {
	return true;
}

bool ChestBlock::canBeSilkTouched() const {
	return false;
}

bool ChestBlock::detachesOnPistonMove(BlockSource& region, const BlockPos& pos) const {
	return true;
}
