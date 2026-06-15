/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "world/level/BlockSource.h"
#include "lovika/tile/TilePlacement.h"
#include "SubChunkT.h"

class ChunkBlockSource : public BlockSource {
public:
	using SubChunk = SubChunkT<8>;
	using RegionSize = BlockPos;

	ChunkBlockSource(RegionSize);
	~ChunkBlockSource();

	BlockID getBlockID(const BlockPos&) override;
	DataID getData(const BlockPos&) override;
	FullBlock getBlockAndData(const BlockPos&) override;

	bool setBlockAndData(const BlockPos&, FullBlock, int updateFlags, Entity* placer = nullptr) override;

	bool setExtraData(const BlockPos&, uint16_t value) override;
	uint16_t getExtraData(const BlockPos&) override;

	int getGrassColor(const BlockPos&) override;

	Biome* tryGetBiome(const BlockPos&) override;

	std::vector<BlockCuboid> blockVolumeHints() const;
	std::vector<BlockCuboid> blockVolumeHints(BlockCuboid) const;

	std::vector<BlockCuboid> blockVolumeHintsRender() const;
	std::vector<BlockCuboid> blockVolumeHintsRender(BlockCuboid) const;

	std::vector<BlockCuboid> blockVolumeHintsShadowed() const;
	std::vector<BlockCuboid> blockVolumeHintsShadowed(BlockCuboid) const;

	FullBlock operator() (BlockPos) const; // BlockGetter functor
	void operator() (BlockPos, FullBlock); // BlockSetter functor

	BlockCuboid bounds() const;

	static RegionSize increaseToSubChunkBounds(RegionSize);
private:
	using ChunkPosIteration = ChunkPos3Iterator<8>;

	static bool isStriclyLessThan(BlockPos a, BlockPos b) {
		return a.x < b.x && a.y < b.y && a.z < b.z;
	}

	int _inRangeSubChunkIndex(SubChunk::ChunkPos pos) const {
		return pos.y * mNumPlaneChunks + pos.z * mSizeInChunks.x + pos.x;
	}

	int _inRangeSubChunkIndex(BlockPos pos) const {
		if (pos.isPositive() && isStriclyLessThan(pos, mBounds.maxExclusive)) {
			return _inRangeSubChunkIndex(SubChunk::ChunkPos(pos));
		}
		return -1;
	}

	SubChunk* chunkAt(BlockPos pos) {
		const int index = _inRangeSubChunkIndex(pos);
		return index >= 0 ? mChunkPtrs[index] : nullptr;
	}

	const SubChunk* chunkAt(BlockPos pos) const {
		const int index = _inRangeSubChunkIndex(pos);
		return index >= 0 ? mChunkPtrs[index] : nullptr;
	}

	std::vector<std::unique_ptr<SubChunk>> mChunkData;
	std::vector<SubChunk*> mChunkPtrs;
	std::vector<byte> mChunksContentIndicator;
	BlockCuboid mBounds;
	SubChunk::ChunkPos mSizeInChunks;
	int mNumPlaneChunks;
};

void paste(ChunkBlockSource&, const PlacementVector&);
Unique<ChunkBlockSource> createChunkBlockSourceFromTilePlacements(const PlacementVector&);
