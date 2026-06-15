/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/BlockPos.h"
#include "world/level/LevelConstants.h"
class BlockGraphicsPack;
class Block;
class BlockSource;

class BlockTessellatorCache {
public:

	BlockTessellatorCache(int CACHE_SIDE_X, int CACHE_SIDE_Y, int CACHE_SIDE_Z, const BlockGraphicsPack&);

	void reset(BlockSource& region, const BlockPos& startPostion);
	BrightnessPair getLightColor(const BlockPos& p);
	const Block& getBlock(const BlockPos& p);
	DataID getData(const BlockPos& pos);
	BlockSource* getRegion() {
		return mRegion;
	}

	const BlockGraphicsPack& getBlockGraphicsPack();


private:
	Brightness _getLightEmission(const Block& block);
	BlockPos _getPosInArray(const BlockPos& p);
	int _getIndexFromPositionInArray(const BlockPos& p);

	int mCacheSide_X;
	int mCacheSide_Y;
	int mCacheSide_Z;
	BlockSource* mRegion = nullptr;
	BlockPos pos = BlockPos::ZERO;	
	const BlockGraphicsPack& mBlockGraphicsPack;

	std::vector<BlockID>			mBlockIDs;
	std::vector<DataID>				mDataIDs;
};
