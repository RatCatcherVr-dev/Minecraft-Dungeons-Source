/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/PodzolBlock.h"
#include "world/level/block/GrassBlock.h"
#include "world/level/material/Material.h"
#include "world/item/McItem.h"
#include "world/Facing.h"

PodzolBlock::PodzolBlock(const std::string& nameId, int id) :
	Block(nameId, id, Material::getMaterial(MaterialType::Dirt)) {
}

int PodzolBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Block::mDirt->mID;
}
