/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/Sapling.h"
#include "world/level/BlockSource.h"
#include "world/level/block/NewLeafBlock.h"
#include "world/level/block/OldLeafBlock.h"
#include "world/level/block/OldLogBlock.h"
#include "world/entity/player/Player.h"
#include "world/level/Level.h"
#include "locale/I18n.h"

const int Sapling::TYPE_DEFAULT = enum_cast(OldLeafBlock::LeafType::Oak);
const int Sapling::TYPE_EVERGREEN = enum_cast(OldLeafBlock::LeafType::Spruce);
const int Sapling::TYPE_BIRCH = enum_cast(OldLeafBlock::LeafType::Birch);
const int Sapling::TYPE_JUNGLE = enum_cast(OldLeafBlock::LeafType::Jungle);
const int Sapling::TYPE_ACACIA = NEW_LEAF_OFFSET + NewLeafBlock::ACACIA_LEAF;
const int Sapling::TYPE_ROOFED_OAK = NEW_LEAF_OFFSET + NewLeafBlock::BIG_OAK_LEAF;

Sapling::Sapling(const std::string& nameId, int id) :
	BushBlock(nameId, id, Material::getMaterial(MaterialType::Plant)) {
	float ss = 0.4f;
	setVisualShape(Vec3(0.5f - ss, 0, 0.5f - ss), Vec3(0.5f + ss, ss * 2, 0.5f + ss));
	mRenderLayer = RENDERLAYER_ALPHATEST_FOILAGE;
}

DataID Sapling::getSpawnResourcesAuxValue(DataID data) const {
	int type = getBlockState(BlockState::MappedType).get<DataID>(data);

	DataID newData = 0;
	getBlockState(BlockState::MappedType).set(newData, type);

	return newData;
}

bool Sapling::isSapling(BlockSource& region, const BlockPos& pos, int type) const {
	const int typeData = getBlockState(BlockState::MappedType).get<int>(region.getData(pos));
	return (region.getBlockID(pos.x, pos.y, pos.z) == mID) && (typeData == type);
}

int Sapling::getVariant(int data) const{
	return getBlockState(BlockState::MappedType).get<int>(data);
}

std::string Sapling::buildDescriptionName(DataID data) const {
	int type = getBlockState(BlockState::MappedType).get<int>(data);
	if(type < 0 || type > TYPE_ROOFED_OAK) {
		type = 0;
	}

	static const std::array<std::string, 6> SAPLING_NAMES = {
		{"oak", "spruce", "birch", "jungle", "acacia", "big_oak"} //D11.PS - Switch needed braces
	};

	return I18n::get(mDescriptionId + "." + SAPLING_NAMES[type] + ".name");
}
