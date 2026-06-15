/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/ItemFrameBlock.h"
#include "world/entity/player/Player.h"
#include "world/item/ItemInstance.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/Facing.h"
#include "world/level/material/Material.h"
#include "world/redstone/Redstone.h"

static const int MAP_GRID_ACHIEVEMENT_SIZE = 3;
// Total detection grid size (for a 3x3 valid grid, we need to detect a total of 5x5 around said map)
static const int MAP_GRID_ACHIEVEMENT_DETECTION_SIZE = 2 * (3 - 1) + 1;

ItemFrameBlock::ItemFrameBlock(const std::string& nameId, int id)
	: EntityBlock(nameId, id, Material::getMaterial(MaterialType::Decoration)) {
	setSolid(false);

	mBlockEntityType = BlockEntityType::ItemFrame;
	mProperties = BlockProperty::BreakOnPush;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

int ItemFrameBlock::getDirection(int data) {
	return Block::mItemFrame->getBlockState(BlockState::Direction).get<int>((DataID)data);
}

int ItemFrameBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	DataID dir = 0;

	switch (face) {
	case Facing::NORTH:
		getBlockState(BlockState::Direction).set(dir, DIR_NORTH);
		break;
	case Facing::SOUTH:
		getBlockState(BlockState::Direction).set(dir, DIR_SOUTH);
		break;
	case Facing::WEST:
		getBlockState(BlockState::Direction).set(dir, DIR_WEST);
		break;
	case Facing::EAST:
		getBlockState(BlockState::Direction).set(dir, DIR_EAST);
		break;
	default:
		break;
	}

	return dir;
}

bool ItemFrameBlock::mayPlace(BlockSource& region, const BlockPos& pos, FacingID face) const {
	if (face == Facing::UP || face == Facing::DOWN) {
		return false;
	}
	
	BlockPos neighbor(pos.x + Facing::STEP_X[Facing::OPPOSITE_FACING[face]], pos.y, pos.z + Facing::STEP_Z[Facing::OPPOSITE_FACING[face]]);
	const Material& material = region.getMaterial(neighbor);
	if (!material.isSolid()) {
		return false;
	}

	return true;
}

void ItemFrameBlock::getShape(int face, AABB& bufferValue) const {
	float min = 2 / 16.0f;
	float max = 14 / 16.0f;
	float depth = 1 / 16.0f;

	bufferValue.set(0, min, min, depth, max, max);
	if (face == 1) {
		bufferValue.set(1 - depth, min, min, 1, max, max);
	}
	else if (face == 2) {
		bufferValue.set(min, min, 0, max, max, depth);
	}
	else if (face == 3) {
		bufferValue.set(min, min, 1 - depth, max, max, 1);
	}

}

const AABB& ItemFrameBlock::getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping) const {
	getShape(region.getData(pos), bufferAABB);
	return bufferAABB;
}

const AABB& ItemFrameBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	if (isClipping) {
		int face = getBlockState(BlockState::Direction).get<int>(region.getData(pos));
		getShape(face, bufferValue);
		return bufferValue.move(Vec3(pos));
	}

	return AABB::EMPTY;
}

bool ItemFrameBlock::isInteractiveBlock() const {
	return true;
}

int ItemFrameBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return McItem::mItemFrame->getId();
}

ItemInstance ItemFrameBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(McItem::mItemFrame, 1, blockData);
}

bool ItemFrameBlock::canBeSilkTouched() const {
	return false;
}

void ItemFrameBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	int dir = getBlockState(BlockState::Direction).get<int>(region.getData(pos));
	dir = Facing::OPPOSITE_FACING[_getFacingID(dir)];
	BlockPos neighbor(pos.x + Facing::STEP_X[dir], pos.y, pos.z + Facing::STEP_Z[dir]);
	const Material& material = region.getMaterial(neighbor);
	if (!material.isSolid()) {
		spawnResources(region, pos, region.getData(pos), 1);
		region.setBlock(pos, BlockID::AIR, Block::UPDATE_ALL);
	}
}

bool ItemFrameBlock::hasComparatorSignal() const {
	return true;
}

bool ItemFrameBlock::use(Player& player, const BlockPos& pos) const {
	return true;
}

int ItemFrameBlock::_getFacingID(int data) const {
	int dir = getBlockState(BlockState::Direction).get<int>(data);
	switch (dir) {
	case DIR_NORTH:
		return Facing::NORTH;
	case DIR_SOUTH:
		return Facing::SOUTH;
	case DIR_WEST:
		return Facing::WEST;
	case DIR_EAST:
		return Facing::EAST;
	default:
		return 0;
	}
}

// Helper function for iterating over the ItemFrame map grid
static void _forEachFrame(const BlockPos& center, int facingDir, std::function<void(const BlockPos&)> callback) {
	BlockPos blockOffset = BlockPos(0, 1, 0);
	switch (facingDir) {
		case Facing::NORTH:
			blockOffset.x = -1;
			break;
		case Facing::EAST:
			blockOffset.z = -1;
			break;
		case Facing::SOUTH:
			blockOffset.x = 1;
			break;
		case Facing::WEST:
			blockOffset.z = 1;
			break;
	}

	for (int yOffset = (MAP_GRID_ACHIEVEMENT_DETECTION_SIZE / 2); yOffset >= -(MAP_GRID_ACHIEVEMENT_DETECTION_SIZE / 2); --yOffset) {
		for (int neighborOffset = -(MAP_GRID_ACHIEVEMENT_DETECTION_SIZE / 2); neighborOffset <= (MAP_GRID_ACHIEVEMENT_DETECTION_SIZE / 2); ++neighborOffset) {
			BlockPos neighborPos = center.offset(blockOffset.x * neighborOffset, blockOffset.y * yOffset, blockOffset.z * neighborOffset);
			callback(neighborPos);
		}
	}
}

ItemFrameBlockEntity* ItemFrameBlock::_getItemFrame(BlockSource& region, const BlockPos& pos) const {
	return nullptr;
}
