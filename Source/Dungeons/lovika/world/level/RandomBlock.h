#pragma once

#include "CommonTypes.h"
#include "LevelGenRandom.h"

struct RandomBlock {
	RandomBlock();
	RandomBlock(FullBlock defaultBlock);

	void add(FullBlock);
	void add(const std::vector<FullBlock>&);
	void add(std::initializer_list<FullBlock>);

	FullBlock operator()() const;
	FullBlock operator()(LevelGenRandom&) const;
private:
	std::vector<FullBlock> mBlocks;
	FullBlock mDefaultBlock;
};

FullBlock randomBlock(const std::vector<FullBlock>&, FullBlock orDefault = FullBlock::AIR);
FullBlock randomBlock(const std::vector<FullBlock>&, LevelGenRandom&, FullBlock orDefault = FullBlock::AIR);
