#include "Dungeons.h"

#include "ObjectGroupBlockSource.h"
#include "world/level/block/Block.h"
#include "world/level/LevelConstants.h"
#include "world/level/BlockPos.h"
#include "world/level/FoliageColor.h"
#include "world/level/material/Material.h"
#include "world/level/biome/Biome.h"

namespace builder {

ObjectGroupBlockSource::ObjectGroupBlockSource(io::ObjectGroup& objGroup)
	: mObjGroup(&objGroup) {
}

ObjectGroupBlockSource::~ObjectGroupBlockSource() {
}

DataID ObjectGroupBlockSource::getData(const BlockPos& p) {
	if (p.y >= 0 && p.y < LEVEL_HEIGHT_DEPRECATED) {
		auto t = getBlockAndData(p);
		return t.data;
	}

	return 0;
}

BlockID ObjectGroupBlockSource::getBlockID(int x, int y, int z) {
	return getBlockID(BlockPos(x, y, z));
}

BlockID ObjectGroupBlockSource::getBlockID(const BlockPos& pos) {
	if (pos.y >= 0 && pos.y < LEVEL_HEIGHT_DEPRECATED) {
		for (auto& object : mObjGroup->objects) {
			if (object.bounds.containsX(pos.x) && object.bounds.containsY(pos.y) && object.bounds.containsZ(pos.z)) {
				BlockPos p(pos.x - object.bounds.minInclusive.x, pos.y - object.bounds.minInclusive.y, pos.z - object.bounds.minInclusive.z);
				FullBlock fullBlock = object.blocks->getBlock(p);
				return fullBlock.id;
			}
		}

		return BlockID::AIR;
	}
	else {
		return BlockID::AIR;
	}
}

const Block& ObjectGroupBlockSource::getBlock(const BlockPos& pos) {
	return *Block::mBlocks[getBlockID(pos)];
}

const Material& ObjectGroupBlockSource::getMaterial(const BlockPos& pos) {
	return getBlock(pos).getMaterial();
}

bool ObjectGroupBlockSource::isSolidBlockingBlock(int x, int y, int z) {
	auto& block = getBlock({ x, y, z });
	return block.getMaterial().isSolidBlocking() && (block.hasProperty(BlockProperty::CubeShaped) || block.hasProperty(BlockProperty::SolidBlocking));
}

bool ObjectGroupBlockSource::isSolidBlockingBlock(const BlockPos& p) {
	return isSolidBlockingBlock(p.x, p.y, p.z);
}

FullBlock ObjectGroupBlockSource::getBlockAndData(const BlockPos& p) {
	if (p.y >= 0 && p.y < LEVEL_HEIGHT_DEPRECATED) {
		for (auto& object : mObjGroup->objects) {
			if (object.bounds.containsX(p.x) && object.bounds.containsY(p.y) && object.bounds.containsZ(p.z)) {
				BlockPos pos(p.x - object.bounds.minInclusive.x, p.y - object.bounds.minInclusive.y, p.z - object.bounds.minInclusive.z);
				return object.blocks->getBlock(pos);
			}
		}
	}
	return FullBlock();
}

bool ObjectGroupBlockSource::isEmptyBlock(int x, int y, int z) {
	return isEmptyBlock(BlockPos(x, y, z));
}

bool ObjectGroupBlockSource::isEmptyBlock(const BlockPos& pos) {
	return getBlockID(pos) == 0;
}

bool ObjectGroupBlockSource::setBlockAndData(int x, int y, int z, FullBlock block, int updateFlags) {
	return setBlockAndData(BlockPos(x, y, z), block, updateFlags);
}

bool ObjectGroupBlockSource::setBlockAndData(const BlockPos& pos, FullBlock block, int updateFlags, Entity* placer) {
	if (pos.y < 0 || pos.y >= LEVEL_HEIGHT_DEPRECATED) {
		return false;
	}

	FullBlock old = getBlockAndData(pos);
	if (old != block) {
		for (auto& object : mObjGroup->objects) {
			if (object.bounds.containsX(pos.x) && object.bounds.containsY(pos.y) && object.bounds.containsZ(pos.z)) {
				BlockPos p(pos.x - object.bounds.minInclusive.x, pos.y - object.bounds.minInclusive.y, pos.z - object.bounds.minInclusive.z);
				object.blocks->setBlock(p, block);
				return true;
			}
		}
	}
	return false;
}

bool ObjectGroupBlockSource::setBlockAndData(const BlockPos& p, BlockID id, DataID data, int updateFlags, Entity* placer) {
	return setBlockAndData(p, FullBlock(id, data), updateFlags);
}

bool ObjectGroupBlockSource::setBlockAndData(int x, int y, int z, BlockID id, DataID data, int updateFlags) {
	return setBlockAndData(x, y, z, FullBlock(id, data), updateFlags);
}

bool ObjectGroupBlockSource::isConsideredSolidBlock(int x, int y, int z) {
	return getBlock({ x, y, z }).isSolid();
}

bool ObjectGroupBlockSource::isConsideredSolidBlock(const BlockPos& pos) {
	return isConsideredSolidBlock(pos.x, pos.y, pos.z);
}

bool ObjectGroupBlockSource::setExtraData(const BlockPos& p, uint16_t extraData) {
	return false;
}

uint16_t ObjectGroupBlockSource::getExtraData(const BlockPos& p) {
	return 0;
}

int ObjectGroupBlockSource::getGrassColor(const BlockPos& pos) {
	return FoliageColor::getGrassColor(1.0f, 1.0f);
}

Biome* ObjectGroupBlockSource::tryGetBiome(const BlockPos& pos) {
	return Biome::ocean.get();
}

Biome& ObjectGroupBlockSource::getBiome(const BlockPos& pos) {
	return *Biome::ocean;
}

}