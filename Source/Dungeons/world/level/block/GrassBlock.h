/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"
#include "legacy/Core/Math/Color.h"

class GrassBlock : public Block {
public:
	static const Brightness MIN_BRIGHTNESS;

	GrassBlock(const std::string& nameId, int id);

	void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	bool onFertilized(BlockSource& region, const BlockPos& pos, Entity* entity) const override;

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
private:
	bool _randomWalk(BlockSource& region, BlockPos& pos, int j) const;
};
