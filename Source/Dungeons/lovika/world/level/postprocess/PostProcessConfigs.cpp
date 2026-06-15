#include "Dungeons.h"
#include "PostProcessConfigs.h"
#include "HeightMapper.h"
#include "lovika/world/level/RandomBlock.h"
#include "world/level/block/Block.h"

namespace postprocess {


//
// WORLD FILL -- Outside fill block generators
//
namespace worldfill { namespace providers {

const BlockProvider& None() {
	static const BlockProvider empty;
	return empty;
}

const BlockProvider& Overworld() {
	static const BlockProvider provider = [grass = FullBlock(Block::mMycelium->mID), dirt = FullBlock(Block::mDirt->mID)] (auto& state) {
		return state.pos.y == state.span.max - 1 ? grass : dirt;
	};
	return provider;
}

const BlockProvider& NewCave() {
	static const BlockProvider provider = Gradient(0, 1, 5, Block::mFillGradient->mID);
	return provider;
}

BlockProvider Uniform(FullBlock block) {
	return [block](auto&) { return block; };
}

BlockProvider Gradient(float minDistance, float slope, float add, BlockID blockId) {
	return [minDistance, slope, add, blockId](auto& state) {
		const float dist = state.span.distance - minDistance;
		const DataID data = dist >= 0 ? Math::clamp(Math::round(add + dist * slope), 0, 15) : 0;
		return FullBlock(blockId, data);
	};
}

BlockProvider BlockForDistance(const std::vector<FullBlock>& blocks, float ditheringAmount /*= 1.0f*/) {
	if (ditheringAmount > 0) {
		return [ditheringAmount, blocks, maxDistance = (int)(blocks.size() - 1)](auto& state) {
			return blocks[Math::clamp(Math::round(state.span.distance + ditheringAmount * state.rnd.nextGaussian()), 0, maxDistance)];
		};
	}
	return [blocks, maxDistance = (int)(blocks.size() - 1)](auto& state) {
		return blocks[std::min(maxDistance, (int)state.span.distance)];
	};
}

BlockProvider FillGradient(const std::vector<int>& rowsPerDataId) {
	std::vector<FullBlock> blocks;
	for (size_t i = 0; i < rowsPerDataId.size(); ++i) {
		blocks.insert(blocks.end(), rowsPerDataId[i], FullBlock(Block::mFillGradient->getId(), i));
	}
	return BlockForDistance(blocks);
}

BlockProvider FillGradient(const std::vector<Pair<int, DataID>>& rowsOfIds) {
	std::vector<FullBlock> blocks;
	for (auto& it : rowsOfIds) {
		blocks.insert(blocks.end(), it.first, FullBlock(Block::mFillGradient->getId(), it.second));
	}
	return BlockForDistance(blocks);
}

}}


//
// DOOR FILL -- Fill the unconnected doors
//
namespace door { namespace providers {

BlockProvider FromWorldFillProvider(const worldfill::BlockProvider& worldFillProvider) {
	return [worldFillProvider](auto& state) {
		return worldFillProvider(state.data);
	};
}

const BlockProvider& RandomDoorFrameBlock() {
	static const BlockProvider provider = [](auto& state) {
		return randomBlock(state.frameBlocks, state.data.rnd);
	};
	return provider;
};

}}


//
// CONFIG -- World fill and Door fill tuples
//
namespace configs {

const Config& NewCave() {
	static const Config config(
		worldfill::providers::NewCave(),
		door::providers::RandomDoorFrameBlock()
	);
	return config;
}

const Config& Overworld() {
	static const Config config = FromWorldFillProvider(postprocess::worldfill::providers::Overworld());
	return config;
}

const Config& OnlyDoors() {
	static const Config config = RandomDoorFrameBlock(/* nothing */);
	return config;
}

Config FromWorldFillProvider(const worldfill::BlockProvider& worldBlockProvider) {
	return Config(worldBlockProvider, door::providers::FromWorldFillProvider(worldBlockProvider));
}

Config RandomDoorFrameBlock(const worldfill::BlockProvider& worldBlockProvider /*= {}*/) {
	return Config(worldBlockProvider, door::providers::RandomDoorFrameBlock());
}

}

}
