/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/EndPortalBlock.h"
#include "world/level/material/Material.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/entity/Entity.h"
#include "world/entity/player/Player.h"
#include "world/level/dimension/Dimension.h"
#include "util/Random.h"

EndPortalBlock::EndPortalBlock(const std::string& nameId, int id)
	: EntityBlock(nameId, id, Material::getMaterial(MaterialType::Portal)) {
	setSolid(false);
	setPushesOutItems(true);

	mProperties = BlockProperty::Portal | BlockProperty::Immovable;
	setLightEmission(1.0f);
	setMapColor(Color::BLACK);
	mRenderLayer = RENDERLAYER_ENDPORTAL;
	mBlockEntityType = BlockEntityType::EndPortal;
}

bool EndPortalBlock::addCollisionShapes(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes, Entity* entity) const {
	Block::addCollisionShapes(region, pos, intersectTestBox, inoutBoxes, entity);
	return false;
}

int EndPortalBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return 0;
}

int EndPortalBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 0;
}

bool EndPortalBlock::isWaterBlocking() const {
	return true;
}

const AABB& EndPortalBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	bufferValue.set((float)pos.x, (float)pos.y, (float)pos.z, (float)pos.x + 1.0f, (float)pos.y + (0.75f), (float)pos.z + 1.0f);
	return bufferValue;
}

bool EndPortalBlock::getCollisionShape(AABB& outAABB, BlockSource& region, const BlockPos& pos, Entity* entity) const {
	getAABB(region, pos, outAABB);
	return true;
}
