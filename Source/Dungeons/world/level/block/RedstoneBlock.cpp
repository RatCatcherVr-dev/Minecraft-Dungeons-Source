#include "Dungeons.h"

#include "RedstoneBlock.h"
#include "world/level/material/Material.h"
#include "world/level/BlockSource.h"
#include "world/level/dimension/Dimension.h"
#include "world/redstone/Redstone.h"
#include "world/level/Level.h"

RedstoneBlock::RedstoneBlock(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::Metal)) {
}

bool RedstoneBlock::shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const {
	return true;
}

void RedstoneBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	onLoaded(region, pos);
}
