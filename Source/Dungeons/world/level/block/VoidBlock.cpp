#include "Dungeons.h"

#include "world/level/block/VoidBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/material/Material.h"
#include "world/level/Level.h"
#include "world/entity/Mob.h"
#include "world/level/biome/Biome.h"
#include "world/level/dimension/Dimension.h"
#include "world/Facing.h"

VoidBlock::VoidBlock(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::Void)) {
	mCanBuildOver = true;
	setSolid(false);
	mRenderLayer = RENDERLAYER_VOID;
	mProperties = BlockProperty::BreakOnPush;
	mCanInstatick = true;
}
