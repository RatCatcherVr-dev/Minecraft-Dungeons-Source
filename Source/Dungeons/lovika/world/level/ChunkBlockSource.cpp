#include "Dungeons.h"

#include "ChunkBlockSource.h"
#include "world/level/block/Block.h"
#include "world/level/BlockPos.h"
#include "world/level/FoliageColor.h"
#include "world/level/biome/Biome.h"
#include "lovika/tile/TilePlacement.h"

enum ChunkContent
{
		EChunkEmpty = 0
	,	EChunkRenderBlocks
	,	EChunkShadowBlocks
	,	EChunk_MAX
};

// Create dummy storage
ChunkBlockSource::ChunkBlockSource(RegionSize size)
	: mBounds(BlockCuboid::fromSize(size))
	, mSizeInChunks(SubChunk::ChunkPos(size - 1) + 1)
{
	DEBUG_ASSERT(size == SubChunk::ChunkPos(size).maxExclusive(), "ChunkBlockSource size must be divisable by chunk size");
	DEBUG_ASSERT(size.isPositive(), "ChunkBlockSource size must be positive");
	mNumPlaneChunks = mSizeInChunks.x * mSizeInChunks.z;
	mChunkPtrs.resize(mSizeInChunks.product());
	mChunksContentIndicator.resize(mChunkPtrs.size());
	memset(mChunksContentIndicator.data(), ChunkContent::EChunkEmpty, mChunksContentIndicator.size());
}

ChunkBlockSource::~ChunkBlockSource() {}

BlockID ChunkBlockSource::getBlockID(const BlockPos& pos) {
	if (auto chunk = chunkAt(pos)) {
		return chunk->getBlock(pos);
	}
	return BlockID(0);
}

DataID ChunkBlockSource::getData(const BlockPos& pos) {
	if (auto chunk = chunkAt(pos)) {
		return chunk->getData(pos);
	}
	return DataID(0);
}

FullBlock ChunkBlockSource::getBlockAndData(const BlockPos& pos) {
	if (auto chunk = chunkAt(pos)) {
		return chunk->getBlockAndData(pos);
	}
	return FullBlock(BlockID(0));
}

bool ChunkBlockSource::setBlockAndData(const BlockPos& pos, FullBlock block, int updateFlags, Entity* placer) {
	const int index = _inRangeSubChunkIndex(pos);
	if (index < 0) {
		return false;
	}

	//mark chunk for contents/shadow
	if (mChunksContentIndicator[index] < ChunkContent::EChunkShadowBlocks)
	{
		const Block* pBlock = Block::mBlocks[static_cast<int>(block.id)];
		if (pBlock != nullptr && pBlock != Block::mAir )
		{
			if (pBlock->getRenderLayerCastsShadow())
			{
				mChunksContentIndicator[index] = ChunkContent::EChunkShadowBlocks;
			}
			else
			{
				mChunksContentIndicator[index] = ChunkContent::EChunkRenderBlocks;
			}
			
		}
	}

	SubChunk* chunk = mChunkPtrs[index];
	if (chunk) {
		chunk->setBlockAndData(pos, block);
	} else {
		auto subChunk = std::make_unique<SubChunk>(pos);
		subChunk->setBlockAndData(pos, block);
		mChunkPtrs[index] = subChunk.get();
		mChunkData.push_back(std::move(subChunk));
	}
	return true;
}

bool ChunkBlockSource::setExtraData(const BlockPos& p, uint16_t extraData) {
	return false;
}

uint16_t ChunkBlockSource::getExtraData(const BlockPos& p) {
	return 0;
}

int ChunkBlockSource::getGrassColor(const BlockPos& pos) {
	return FoliageColor::getGrassColor(1.0f, 1.0f);
}

Biome* ChunkBlockSource::tryGetBiome(const BlockPos& pos) {
	return Biome::ocean.get();
}

std::vector<BlockCuboid> ChunkBlockSource::blockVolumeHintsRender() const
{
	std::vector<BlockCuboid> out;
	for (auto&& chunk : mChunkData) {
		auto chunkPos = chunk->pos();
		const int index = _inRangeSubChunkIndex(chunkPos);
		if (mChunksContentIndicator[index] > ChunkContent::EChunkEmpty)
			out.emplace_back(chunkPos.min(), chunkPos.maxExclusive());
	}
	return out;
}

std::vector<BlockCuboid> ChunkBlockSource::blockVolumeHintsRender(BlockCuboid bounds) const
{
	bounds = bounds.intersection(mBounds);

	std::vector<BlockCuboid> out;
	for (auto&& chunkPos : ChunkPosIteration::rangeIncl(bounds)) {
		auto min = chunkPos.min();
		const int index = _inRangeSubChunkIndex(chunkPos);
		if (index >= 0 && mChunkPtrs[index] && mChunksContentIndicator[index] > ChunkContent::EChunkEmpty)
			out.emplace_back(min, chunkPos.maxExclusive());

	}
	return out;
}

std::vector<BlockCuboid> ChunkBlockSource::blockVolumeHintsShadowed() const
{
	std::vector<BlockCuboid> out;
	for (auto&& chunk : mChunkData) {
		auto chunkPos = chunk->pos();
		const int index = _inRangeSubChunkIndex(chunkPos);
		if (mChunksContentIndicator[index] == ChunkContent::EChunkShadowBlocks)
			out.emplace_back(chunkPos.min(), chunkPos.maxExclusive());
	}
	return out;
}

std::vector<BlockCuboid> ChunkBlockSource::blockVolumeHintsShadowed(BlockCuboid bounds) const
{
	bounds = bounds.intersection(mBounds);

	std::vector<BlockCuboid> out;
	for (auto&& chunkPos : ChunkPosIteration::rangeIncl(bounds)) {
		auto min = chunkPos.min();
		const int index = _inRangeSubChunkIndex(chunkPos);		 
		if (index >= 0 && mChunkPtrs[index] && mChunksContentIndicator[index] == ChunkContent::EChunkShadowBlocks)
			out.emplace_back(min, chunkPos.maxExclusive());
		
	}
	return out;
}

FullBlock ChunkBlockSource::operator()(BlockPos pos) const {
	if (auto chunk = chunkAt(pos)) {
		return chunk->getBlockAndData(pos);
	}
	return FullBlock(BlockID(0));
}

void ChunkBlockSource::operator()(BlockPos pos, FullBlock block) {
	setBlockAndData(pos, block, Block::UPDATE_NONE);
}

std::vector<BlockCuboid> ChunkBlockSource::blockVolumeHints() const {
	std::vector<BlockCuboid> out;
	for (auto&& chunk : mChunkData) {
		auto chunkPos = chunk->pos();
		out.emplace_back(chunkPos.min(), chunkPos.maxExclusive());
	}
	return out;
}

std::vector<BlockCuboid> ChunkBlockSource::blockVolumeHints(BlockCuboid bounds) const {
	bounds = bounds.intersection(mBounds);

	std::vector<BlockCuboid> out;
	for (auto&& chunkPos : ChunkPosIteration::rangeIncl(bounds)) {
		auto min = chunkPos.min();
		if (chunkAt(min)) {
			out.emplace_back(min, chunkPos.maxExclusive());
		}
	}
	return out;
}

BlockCuboid ChunkBlockSource::bounds() const {
	return mBounds;
}

BlockPos ChunkBlockSource::increaseToSubChunkBounds(BlockPos size) {
	return SubChunk::ChunkPos(size).maxExclusive();
}

void paste(ChunkBlockSource& region, const PlacementVector& tilePlacements) {
	for (auto& tp : tilePlacements) {
		auto& blocks = tp.tile().blocks();
		auto offset = tp.placement().position;

		for (auto pos : BlockPosIteration::range(blocks.size())) {
			auto block = blocks.getBlock(pos);
			if (block.id != 0) {
				region.setBlockAndData(pos + offset, block, Block::UPDATE_NONE);
			}
		}
	}
}

Unique<ChunkBlockSource> createChunkBlockSourceFromTilePlacements(const PlacementVector& placements) {
	auto regionSize = ChunkBlockSource::increaseToSubChunkBounds(calculateBounds(placements).maxExclusive.offset(32, 0, 32));
	auto region = std::make_unique<ChunkBlockSource>(regionSize);
	paste(*region, placements);
	return region;
}
