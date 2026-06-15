/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/ChalkboardBlock.h"

#include "world/Direction.h"
#include "world/entity/player/Player.h"
#include "world/Facing.h"
#include "world/item/McItem.h"
#include "world/item/ItemInstance.h"
#include "world/level/BlockSource.h"
#include "world/level/material/Material.h"
#include "world/level/Level.h"

ChalkboardBlock::ChalkboardBlock(const std::string& nameId, int id)
	: EntityBlock(nameId, id, Material::getMaterial(MaterialType::Wood)) {

	float r = 4 / 16.0f;
	float h = 16 / 16.0f;

	setVisualShape(Vec3(0.5f - r, 0, 0.5f - r), Vec3(0.5f + r, h, 0.5f + r));
	setSolid(false);

	mBlockEntityType = BlockEntityType::Chalkboard;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

const AABB& ChalkboardBlock::getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping) const {
	ChalkboardBlockEntity* blockEntity = getBlockEntity(region, pos);
	DEBUG_ASSERT(blockEntity, "Chalkboard block with no BlockEntity");
	return bufferAABB;
}

void ChalkboardBlock::_getShape(ChalkboardSize size, bool isOnGround, int dir, AABB& bufferValue) const {
	float heightBottom = 4.f / 16.f;
	static const float heightTop = 1.f;
	static const float widthLeft = 0 / 16.0f;
	static const float widthRight = 16 / 16.0f;

	static const float depth = 2 / 16.0f;

	bufferValue.set(0, 0, 0, 1, 1, 1);

	if (!isOnGround) {
		// Make non-1x1 full block size for convenience
// 		if (size != ChalkboardSize::OnebyOne) {
// 			heightBottom = 0.f;
// 		}

		if (dir == Direction::NORTH) {
			bufferValue.set(widthLeft, heightBottom, 1 - depth, widthRight, heightTop, 1);
		}
		if (dir == Direction::SOUTH) {
			bufferValue.set(widthLeft, heightBottom, 0, widthRight, heightTop, depth);
		}
		if (dir == Direction::WEST) {
			bufferValue.set(1 - depth, heightBottom, widthLeft, 1, heightTop, widthRight);
		}
		if (dir == Direction::EAST) {
			bufferValue.set(0, heightBottom, widthLeft, depth, heightTop, widthRight);
		}
	}
}

const AABB& ChalkboardBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	return AABB::EMPTY;
}

bool ChalkboardBlock::checkIsPathable(Entity& entity, const BlockPos& lastPathPos, const BlockPos& pathPos) const {
	return true;
}

ItemInstance ChalkboardBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int) const {
	return ItemInstance(McItem::mChalkboard, 1, 0);
}

void ChalkboardBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	EntityBlock::neighborChanged(region, pos, neighborPos);
	ChalkboardBlockEntity* currentChalkboard = getBlockEntity(region, pos);

	bool remove = false;

	if (currentChalkboard == nullptr) {
		// Remove if our block entity is gone
		remove = true;
	}

	if (remove) {
		region.setBlock(pos, BlockID::AIR, Block::UPDATE_ALL);
	}
}

int ChalkboardBlock::getVariant(int data) const {
	//the default getVariant returns data which in our case stores facing and is wrong
	return 0;
}

ChalkboardBlockEntity* ChalkboardBlock::getBlockEntity(BlockSource& region, const BlockPos& pos) {
	return nullptr;
}

void ChalkboardBlock::spawnResources(BlockSource& region, const BlockPos& pos, int data, float odds, int bonusLootLevel) const {
	 // Do nothing, PlayerWillDestroy spawns this 
}

bool ChalkboardBlock::use(Player& player, const BlockPos& pos) const {
	return true;
}

bool ChalkboardBlock::canBeSilkTouched() const {
	return false;
}
