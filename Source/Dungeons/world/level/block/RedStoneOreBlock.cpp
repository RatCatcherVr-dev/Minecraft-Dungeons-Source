/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/RedStoneOreBlock.h"
#include "world/level/material/Material.h"
#include "world/level/Level.h"
#include "world/level/BlockSource.h"
#include "world/entity/player/Player.h"
#include "world/item/McItem.h"

RedStoneOreBlock::RedStoneOreBlock(const std::string& nameId, int id, bool lit)
	: Block(nameId, id, Material::getMaterial(MaterialType::Stone)) {
	if (lit) {
		setTicking(true);
	}

	mLit = lit;
}

bool RedStoneOreBlock::attack(Player* player, const BlockPos& pos) const {
	_interact(player->getRegion(), pos);
	return Block::attack(player, pos);
}

void RedStoneOreBlock::onStepOn(Entity& entity, const BlockPos& pos) const {
	_interact(entity.getRegion(), pos);
	Block::onStepOn(entity, pos);
}

bool RedStoneOreBlock::use(Player& player, const BlockPos& pos) const {
	_interact(player.getRegion(), pos);
	return Block::use(player, pos);
}

int RedStoneOreBlock::getTickDelay() const {
	return 30;
}

void RedStoneOreBlock::tick(BlockSource& region, const BlockPos& pos, Random& random) const {
	if (mID == Block::mLitRedStoneOre->mID) {
		region.setBlock(pos.x, pos.y, pos.z, Block::mRedStoneOre->mID, Block::UPDATE_CLIENTS);
	}
}

int RedStoneOreBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return McItem::mRedStone->getId();
}

int RedStoneOreBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 4 + random.nextInt(2 + bonusLootLevel);
}

int RedStoneOreBlock::getExperienceDrop(Random& random) const {
	return random.nextInt(1, 5);
}

ItemInstance RedStoneOreBlock::getSilkTouchItemInstance(DataID data) const {
	return ItemInstance(Block::mRedStoneOre);
}

void RedStoneOreBlock::_interact(BlockSource& region, const BlockPos& pos) const {
	if (mID == Block::mRedStoneOre->mID) {
		region.setBlock(pos, Block::mLitRedStoneOre->mID, Block::UPDATE_CLIENTS);
	}
}
