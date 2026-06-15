/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/

#include "Dungeons.h"

#include "world/level/block/StructureBlock.h"
#include "world/level/material/Material.h"
#include "world/level/block/entity/BlockEntity.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/level/LevelConstants.h"
#include "world/entity/player/Player.h"

StructureBlock::StructureBlock(const std::string& nameId, int id) :
	EntityBlock(nameId, id, Material::getMaterial(MaterialType::Metal)) {

	mBlockEntityType = BlockEntityType::StructureBlock;
	mProperties = BlockProperty::Immovable;
}

bool StructureBlock::use(Player&, const BlockPos&) const {
	return true;
}

int StructureBlock::getResourceCount(Random&, int data, int bonusLootLevel) const {
	return 0;
}

ItemInstance StructureBlock::asItemInstance(BlockSource&, const BlockPos&, int blockData) const {
	return ItemInstance(this, 1, 0);
}
