/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class FallingBlock;
class Color;
class Material;

class HeavyBlock : public Block {
public:
	HeavyBlock(const std::string& nameId, int id, const Material& material );

	void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	int getTickDelay() const;

	virtual Color getDustColor(DataID data) const = 0;

	virtual bool falling() const;
	virtual void onLand(BlockSource& region, const BlockPos& pos) const;

	virtual bool isFree(BlockSource& region, const BlockPos& pos) const;
protected:
	virtual void startFalling(BlockSource& region, const BlockPos& pos, const Block* oldBlock, bool creative) const {}
	void checkSlide(BlockSource& region, const BlockPos& pos) const;
private:
	BlockPos _findBottomSlidingBlock(BlockSource& region, const BlockPos& pos) const;
};
