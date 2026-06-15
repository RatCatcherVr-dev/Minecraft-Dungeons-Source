/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/EnchantingTableBlock.h"
#include "world/level/material/Material.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/Facing.h"

EnchantingTableBlock::EnchantingTableBlock(const std::string& nameId, int id)
	: EntityBlock(nameId, id, Material::getMaterial(MaterialType::Stone)) {
	setVisualShape( Vec3::ZERO, Vec3(1, 12 / 16.0f, 1));
	setLightBlock(Brightness::MAX);
	setLightEmission(0.8f);
	setDestroyTime(5);

	mBlockEntityType = BlockEntityType::EnchantingTable;
	
	mProperties = BlockProperty::Immovable;

	setSolid(false);
	setPushesOutItems(true);
}

bool EnchantingTableBlock::use(Player& player, const BlockPos& pos) const {
	return true;
}

bool EnchantingTableBlock::isCraftingBlock() const {
	return true;
}

bool EnchantingTableBlock::canBeSilkTouched() const {
	return false;
}
