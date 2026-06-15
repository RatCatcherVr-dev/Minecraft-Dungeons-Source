#include "Dungeons.h"

#include "ObserverBlock.h"
#include "world/level/material/Material.h"
#include "client/renderer/texture/TextureAtlasItem.h"
#include "CommonTypes.h"
#include "world/Facing.h"
#include "world/entity/Mob.h"
#include "world/entity/player/Player.h"
#include "world/level/Level.h"
#include "../BlockSource.h"

ObserverBlock::ObserverBlock(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::Stone))
{
	setSolid(true);
	mProperties = BlockProperty::Unspecified;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

ObserverBlock::~ObserverBlock() {

}

ItemInstance ObserverBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	DataID data = 0;
	getBlockState(BlockState::FacingDirection).set(data, Facing::SOUTH);
	return ItemInstance(Block::mObserver, 1, data);
}

FacingID ObserverBlock::getMappedFace(FacingID face, int data) const {
	static const FacingID mDirToFace[6][6] {
		// face DOWN top, UP top, NORTH eye, SOUTH butt, WEST west, EAST east						// direction \/
		{Facing::NORTH, Facing::SOUTH, Facing::UP, Facing::DOWN, Facing::WEST, Facing::WEST},	//		facing DOWN = 0, (east/west can flip)
		{Facing::SOUTH, Facing::NORTH, Facing::UP, Facing::DOWN, Facing::WEST, Facing::WEST},	//		facing UP (e/w u/d can flip)
		{Facing::DOWN, Facing::UP, Facing::NORTH, Facing::SOUTH, Facing::WEST, Facing::EAST},	//		NORTH / default
		{Facing::DOWN, Facing::UP, Facing::SOUTH, Facing::NORTH, Facing::WEST, Facing::EAST},	//		SOUTH = 3,
		{Facing::DOWN, Facing::UP, Facing::EAST, Facing::WEST, Facing::NORTH, Facing::SOUTH},	//		WEST = 4,
		{Facing::DOWN, Facing::UP, Facing::EAST, Facing::WEST, Facing::SOUTH, Facing::NORTH},	//		EAST = 5	
	};
	
	int dir = getBlockState(BlockState::FacingDirection).get<int>(data);
	DEBUG_ASSERT(dir < 6, "invalid direction");

	return mDirToFace[dir][face];
}

int ObserverBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	return getPlacementFacingAll(by, pos, 180.0f);
}

bool ObserverBlock::shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const {
	DataID data = region.getData(pos);
	int dir = (data & 0x7);
	return direction == dir;
}
