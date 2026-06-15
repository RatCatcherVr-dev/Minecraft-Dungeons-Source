/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class TntBlock : public Block {
public:
	TntBlock(const std::string& nameId, int id);

	virtual int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;

	virtual void onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const override;
	virtual void onPlace(BlockSource& region, const BlockPos& pos) const override;

	virtual bool use(Player& player, const BlockPos& pos) const override;

	virtual bool canProvideSupport(BlockSource& region, const BlockPos& pos, FacingID face, BlockSupportType type) const override;
private:
	bool shouldExplode(const DataID& data) const;
};
