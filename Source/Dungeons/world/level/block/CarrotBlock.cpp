/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/CarrotBlock.h"
#include "world/item/McItem.h"

CarrotBlock::CarrotBlock(const std::string& nameId, int id)
	: CropBlock(nameId, id) {
	mProperties = BlockProperty::BreakOnPush;
}

McItem* CarrotBlock::getBaseSeed() const {
	return McItem::mCarrot;
}

int CarrotBlock::getBasePlantId() const {
	return McItem::mCarrot->getId();
}

int CarrotBlock::getVariant(int data) const {
	auto growth = getBlockState(BlockState::Growth).get<int>(data);
	if (growth < MAX_GROWTH) {
		if (data == 6) {
			data = 5;
		}
		return data >> 1;
	}
	else {
		return 3;
	}
}

void CarrotBlock::spawnResources(BlockSource& region, const BlockPos& pos, int data, float odds, int bonusLootLevel) const {
	CropBlock::spawnResources(region, pos, data, odds, bonusLootLevel);
}

int CarrotBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 1 + random.nextInt(bonusLootLevel + 1);
}
