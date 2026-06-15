#include "Dungeons.h"
#include "RandomBlock.h"

RandomBlock::RandomBlock()
	: RandomBlock(FullBlock::AIR) {
}

RandomBlock::RandomBlock(FullBlock defaultBlock)
	: mDefaultBlock(defaultBlock) {
}

void RandomBlock::add(FullBlock block) {
	mBlocks.push_back(block);
}

void RandomBlock::add(const std::vector<FullBlock>& blocks) {
	mBlocks.insert(end(mBlocks), begin(blocks), end(blocks));
}

void RandomBlock::add(std::initializer_list<FullBlock> blocks) {
	mBlocks.insert(end(mBlocks), blocks);
}

FullBlock RandomBlock::operator()() const {
	static LevelGenRandom rnd;
	return operator()(rnd);
}

FullBlock RandomBlock::operator()(LevelGenRandom& rnd) const {
	return randomBlock(mBlocks, mDefaultBlock);
}

FullBlock randomBlock(const std::vector<FullBlock>& blocks, FullBlock orDefault /*= FullBlock::AIR*/) {
	static LevelGenRandom rnd;
	return randomBlock(blocks, rnd, orDefault);
}

FullBlock randomBlock(const std::vector<FullBlock>& blocks, LevelGenRandom& rnd, FullBlock orDefault /*= FullBlock::AIR*/) {
	return blocks.empty() ? orDefault : blocks[rnd.nextInt(blocks.size())];
}
