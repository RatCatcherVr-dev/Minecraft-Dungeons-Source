#include "Dungeons.h"

#include "world/level/block/EndGatewayBlock.h"
#include "world/level/material/Material.h"
#include "util/Random.h"
#include "world/level/BlockSource.h"
#include "world/entity/Entity.h"
#include "world/item/ItemInstance.h"
#include "world/level/block/entity/BlockEntity.h"

EndGatewayBlock::EndGatewayBlock(const std::string& nameId, int id)
	: EntityBlock(nameId, id, Material::getMaterial(MaterialType::Portal)) {
	setSolid(false);
	setPushesOutItems(true);
	mProperties = BlockProperty::Portal | BlockProperty::Immovable;
	mBlockEntityType = BlockEntityType::EndGateway;
	setLightEmission(1.0f);
	setMapColor(Color::BLACK);
	mRenderLayer = BlockRenderLayer::RENDERLAYER_ENDPORTAL;
}

int EndGatewayBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 0;
}

bool EndGatewayBlock::addCollisionShapes(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes, Entity* entity) const {
	if (entity) {
		const BlockEntity* blockEntity = region.getBlockEntity(pos);
		if (!blockEntity || !(blockEntity->getType() == BlockEntityType::EndGateway)) {
			return false;
		}
// 		const EndGatewayBlockEntity* gateway = static_cast<const EndGatewayBlockEntity*>(blockEntity);
// 		if (!gateway->canTeleport(entity, region)) {
// 			return Block::addCollisionShapes(region, pos, intersectTestBox, inoutBoxes, entity);
// 		}
	}
	return false;
}


bool EndGatewayBlock::shouldRenderFace(BlockSource& region, const BlockPos& pos, FacingID face, const AABB& shape) const {
	const Block& block = region.getBlock(pos.relative(face));
	return !block.isSolid() && block.mID != Block::mEndGateway->mID;
}

bool EndGatewayBlock::isWaterBlocking() const {
	return true;
}

ItemInstance EndGatewayBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance();
}
