#pragma once
#include "CommonTypes.h"
#include "world/level/BlockPos.h"

class BlockRegion {
public:
    using RegionSize = BlockPos;
    using RegionPos = BlockPos;

    explicit BlockRegion(const RegionSize& size);
	BlockRegion(const BlockRegion& other) = default;
	BlockRegion(BlockRegion&& other) noexcept;
	virtual ~BlockRegion() {}

    FullBlock getBlock(int x, int y, int z) const;
    FullBlock getBlock(const RegionPos&) const;
    BlockID getBlockId(int x, int y, int z) const;
    BlockID getBlockId(const RegionPos&) const;

    void setBlock(int x, int y, int z, const FullBlock& block);
    void setBlock(const RegionPos&, const FullBlock& block);	
    void setBlock(int x, int y, int z, BlockID block);
    void setBlock(const RegionPos&, BlockID block);

	//only use these if your sure about your boundaries
	void setBlock(int linearIndex, const FullBlock& block) { mBlocks[linearIndex] = block; };
	void setBlock(int linearIndex, BlockID block) { mBlocks[linearIndex] = FullBlock(block); };

    RegionSize size() const;

	bool operator==(const BlockRegion& rhs) const;
	bool operator!=(const BlockRegion& rhs) const;

	FullBlock operator() (BlockPos) const; // BlockGetter functor
	void operator() (BlockPos, FullBlock); // BlockSetter functor
protected:
    int _toLinearIndex(int x, int y, int z) const;
    bool _inRange(int x, int y, int z) const;

    RegionSize mSize;
    int mXZPlaneSize;
    std::vector<FullBlock> mBlocks;
};

unsigned dataSizeInBytes(const BlockRegion&);
