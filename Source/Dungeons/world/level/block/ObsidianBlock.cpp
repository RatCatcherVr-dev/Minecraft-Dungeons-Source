/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/ObsidianBlock.h"
#include "world/level/material/Material.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"

ObsidianBlock::ObsidianBlock(const std::string& nameId, int id, bool isGlowing)
	: Block(nameId, id, Material::getMaterial(MaterialType::Stone))
	, mIsGlowing(isGlowing) { 
	mBrightnessGamma = 2.f;
	 
	mProperties = BlockProperty::Immovable | BlockProperty::CubeShaped;
}

int ObsidianBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 1;
}

int ObsidianBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Block::mObsidian->mID;
}
