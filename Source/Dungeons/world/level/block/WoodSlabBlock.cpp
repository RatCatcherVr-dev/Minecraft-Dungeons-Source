/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/WoodSlabBlock.h"

#include "world/level/block/WoodBlock.h"
#include "world/level/material/Material.h"
#include "world/level/BlockSource.h"
#include "world/item/ItemInstance.h"
#include "world/entity/player/Player.h"
#include "world/level/Level.h"
#include "world/Facing.h"

WoodSlabBlock::WoodSlabBlock(const std::string& nameId, int id, bool fullSize) :
	SlabBlock(nameId, id, fullSize, Material::getMaterial(MaterialType::Wood)) {
	mSlabType = SlabBlock::SlabType::WoodSlab;
}

int WoodSlabBlock::getVariant(int data) const {
	return getBlockState(BlockState::MappedType).get<int>(data);
}

int WoodSlabBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Block::mWoodenSlab->mID;
}

DataID WoodSlabBlock::getSpawnResourcesAuxValue(DataID data) const {
	return getBlockState(BlockState::MappedType).get<int>(data);
}

ItemInstance WoodSlabBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	int type = getBlockState(BlockState::MappedType).get<int>(blockData);

	DataID data = 0;
	getBlockState(BlockState::MappedType).set(data, type);

	return ItemInstance(Block::mWoodenSlab, 1, data);
}

bool WoodSlabBlock::isValidAuxValue(int auxValue) const {
	return WoodBlock::isValidWoodMaterial(auxValue);
}

std::string WoodSlabBlock::buildDescriptionName(DataID data) const {
	int type = getBlockState(BlockState::MappedType).get<int>(data);
	if(type < 0 || type >= 7) {
		type = 0;
	}

	static const std::array<std::string, 7> WOOD_NAMES = {
		{"oak", "spruce", "birch", "jungle", "acacia", "big_oak" } //D11.PS - Switch needed braces
	};

	return I18n::get(mDescriptionId + "." + WOOD_NAMES[type] + ".name");
}
