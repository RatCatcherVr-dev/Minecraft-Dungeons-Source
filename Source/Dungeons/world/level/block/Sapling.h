/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/BushBlock.h"

class Sapling : public BushBlock {
public:
	static const int NEW_LEAF_OFFSET = 4;
	static const int TYPE_DEFAULT;
	static const int TYPE_EVERGREEN;
	static const int TYPE_BIRCH;
	static const int TYPE_JUNGLE;
	static const int TYPE_ACACIA;
	static const int TYPE_ROOFED_OAK;

	Sapling(const std::string& nameId, int id);

	int getVariant(int data) const override;

	bool isSapling(BlockSource& region, const BlockPos& pos, int type) const;

	virtual std::string buildDescriptionName(DataID data) const override;
protected:
	DataID getSpawnResourcesAuxValue(DataID data) const override;
private:
	static const int TYPE_MASK = 7;
	static const int AGE_BIT = 8;
};
