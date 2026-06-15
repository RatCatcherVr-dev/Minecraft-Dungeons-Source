#include "Dungeons.h"
#include "client/renderer/block/BlockTessellatorCache.h"
#include "world/level/block/Block.h"
#include "world/level/BlockPos.h"
#include "world/level/BlockSource.h"

const BlockID UnsetBlockID = (BlockID)-1;

BlockTessellatorCache::BlockTessellatorCache(int CACHE_SIDE_X, int CACHE_SIDE_Y, int CACHE_SIDE_Z, const BlockGraphicsPack& blockGraphicsPack)
	: mBlockGraphicsPack(blockGraphicsPack)
{
	mCacheSide_X = CACHE_SIDE_X + 4;
	mCacheSide_Y = CACHE_SIDE_Y + 4;
	mCacheSide_Z = CACHE_SIDE_Z + 4;

	mBlockIDs.resize(mCacheSide_X*mCacheSide_Y*mCacheSide_Z);
	mDataIDs.resize(mCacheSide_X*mCacheSide_Y*mCacheSide_Z);

	
}

void BlockTessellatorCache::reset(BlockSource& region, const BlockPos& startPostion){
	mRegion = &region;
	// we can always read one outside the chunk on each side
	pos = startPostion.west(2).north(2).below(2);
	
	mBlockIDs.assign(mBlockIDs.size(), UnsetBlockID);
	mDataIDs.assign(mDataIDs.size(), 255);
}

BrightnessPair BlockTessellatorCache::getLightColor(const BlockPos& p) {
	if (p.y >= mRegion->getMaxHeight()) {
		return{ Brightness::MAX, Brightness::MIN }; //just skylight
	}
	int index = _getIndexFromPositionInArray(_getPosInArray(p));
	
	const Block& block = getBlock(p);
	auto light = mRegion->getLightColor(p, _getLightEmission(block));

	if(light.sky == 0 && light.block == 0) {
		if(block.hasProperty(BlockProperty::HalfSlab)) {
			const Block& blockBelow = getBlock(p.below());
			// We need to get the raw value here, can't used the cached one since that one
			// can be changed
			light = mRegion->getLightColor(p.below(), _getLightEmission(blockBelow));
		}
	}

	return light;
}

Brightness BlockTessellatorCache::_getLightEmission(const Block& block) {
	return Block::getLightEmission(block.mID);
}

const Block& BlockTessellatorCache::getBlock(const BlockPos& p) {
	if(p.y >= mRegion->getMaxHeight() || p.y < 0) {
		return *Block::mAir;
	}

	int index = _getIndexFromPositionInArray(_getPosInArray(p));
	BlockID cachedBlock = mBlockIDs[index];
		
	if (cachedBlock != UnsetBlockID) {
		return cachedBlock.getBlock();
	}

	auto checkedBlock = mRegion->getBlockID(p);
	checkf(&checkedBlock.getBlock(), TEXT("NO NULL POINTERS"));
	mBlockIDs[index] = checkedBlock;
	return checkedBlock.getBlock();
}

BlockPos BlockTessellatorCache::_getPosInArray(const BlockPos& p) {
	DEBUG_ASSERT(p.x >= pos.x && p.y >= pos.y && p.z >= pos.z, "Point is outside cache, should be possible.");
	DEBUG_ASSERT(p.x - pos.x < mCacheSide_X && p.y - pos.y < mCacheSide_Y && p.z - pos.z < mCacheSide_Z, "Point is outside cache, should be possible.");

	return p - pos;
}

int BlockTessellatorCache::_getIndexFromPositionInArray(const BlockPos& p) {
	DEBUG_ASSERT(p.x >= 0 && p.y >= 0 && p.z >= 0, "Point is outside cache, should be possible.");
	DEBUG_ASSERT(p.x < mCacheSide_X && p.y < mCacheSide_Y && p.z < mCacheSide_Z, "Point is outside cache, should be possible.");
	// TODO: Check if this order is optimal based on rendering ordering.
	//       Since we read all sides, perhaps it makes no difference.
	return (p.x * mCacheSide_Y * mCacheSide_Z) + (p.y * mCacheSide_Z) + p.z;
}

DataID BlockTessellatorCache::getData(const BlockPos& _pos) {
	if(_pos.y >= mRegion->getMaxHeight() && _pos.y < 0) {
		return 0;
	}

	int index = _getIndexFromPositionInArray(_getPosInArray(_pos));
	DataID cachedData = mDataIDs[index];
	if(cachedData != 255) {
		return cachedData;
	}

	DataID checkedData = mRegion->getData(_pos);
	mDataIDs[index] = checkedData;
	return checkedData;
}

const BlockGraphicsPack& BlockTessellatorCache::getBlockGraphicsPack() {
	return mBlockGraphicsPack;
}

