#pragma once

#include "world/level/BlockPos.h"

template <unsigned int ChunkWidth>
struct SubChunkT {
	static_assert((ChunkWidth & (ChunkWidth - 1)) == 0, "ChunkWidth must be a power of two");
	enum {
		Width = ChunkWidth,
		WidthMask = ChunkWidth - 1,
		NumPlaneBlocks = ChunkWidth * ChunkWidth,
		NumBlocks = ChunkWidth * ChunkWidth * ChunkWidth
	};

	struct Pos {
		Pos(int x, int y, int z)
			: x(x & WidthMask)
			, y(y & WidthMask)
			, z(z & WidthMask) {
		}
		Pos(BlockPos pos)
			: x(pos.x & WidthMask)
			, y(pos.y & WidthMask)
			, z(pos.z & WidthMask) {
		}
		int index() const {
			return y * NumPlaneBlocks + z * Width + x;
		}
		int x, y, z;
	};

	struct ChunkPos {
		ChunkPos() : x(0), y(0), z(0) {}

		ChunkPos(int x, int y, int z)
			: x(x)
			, y(y)
			, z(z) {
		}

		ChunkPos(BlockPos pos)
			: x(pos.x / Width)
			, y(pos.y / Width)
			, z(pos.z / Width) {
		}

		ChunkPos operator+(int offset) const {
			return ChunkPos(x + offset, y + offset, z + offset);
		}

		ChunkPos operator-(int offset) const {
			return ChunkPos(x - offset, y - offset, z - offset);
		}

		ChunkPos operator+(const ChunkPos& rhs) const {
			return ChunkPos(x + rhs.x, y + rhs.y, z + rhs.z);
		}

		ChunkPos operator-(const ChunkPos& rhs) const {
			return ChunkPos(x - rhs.x, y - rhs.y, z - rhs.z);
		}

		int product() const {
			return x * y * z;
		}

		BlockPos min() const {
			return BlockPos(x, y, z) * Width;
		}

		BlockPos max() const {
			return min() + WidthMask;
		}

		BlockPos maxExclusive() const {
			return BlockPos(x + 1, y + 1, z + 1) * Width;
		}

		static ChunkPos min(ChunkPos a, ChunkPos b) {
			return ChunkPos(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
		}

		static ChunkPos max(ChunkPos a, ChunkPos b) {
			return ChunkPos(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
		}

		int x, y, z;
	};

	SubChunkT(ChunkPos pos)
		: mPos(pos) {
	}
	SubChunkT(const SubChunkT&) = delete;
	SubChunkT& operator=(const SubChunkT&) = delete;

	FullBlock getBlockAndData(Pos pos) const {
		return mBlocks[pos.index()];
	}

	BlockID getBlock(Pos pos) const {
		return mBlocks[pos.index()].id;
	}

	DataID getData(Pos pos) const {
		return mBlocks[pos.index()].data;
	}

	void setBlockAndData(Pos pos, FullBlock block) {
		mBlocks[pos.index()] = block;
	}

	void setBlock(Pos pos, BlockID blockId) {
		mBlocks[pos.index()].id = blockId;
	}

	void setData(Pos pos, DataID data) {
		mBlocks[pos.index()].data = data;
	}

	ChunkPos pos() const {
		return mPos;
	}
private:
	ChunkPos mPos;
	std::array<FullBlock, NumBlocks> mBlocks;
};


//
// Iterator
//
template <unsigned int ChunkWidth>
class ChunkPos3Iterator {
	typedef typename SubChunkT<ChunkWidth>::ChunkPos ChunkPos;

	ChunkPos3Iterator(const ChunkPos& min, const ChunkPos& maxExclusive, uint64_t index = 0)
		: mMin(ChunkPos::min(min, maxExclusive))
		, mMax(ChunkPos::max(min, maxExclusive))
		, mPos(mMin)
		, mIdx(index) {
	}
public:
	static ChunkPos3Iterator range(const ChunkPos& maxExclusive) {
		return ChunkPos3Iterator(ChunkPos(0, 0, 0), maxExclusive);
	}
	static ChunkPos3Iterator range(const ChunkPos& min, const ChunkPos& maxExclusive) {
		return ChunkPos3Iterator(min, maxExclusive);
	}
	static ChunkPos3Iterator rangeIncl(const ChunkPos& min, const ChunkPos& maxInclusive) {
		return ChunkPos3Iterator(min, ChunkPos(maxInclusive.x + 1, maxInclusive.y + 1, maxInclusive.z + 1));
	}
	static ChunkPos3Iterator rangeIncl(const BlockPos& maxInclusive) {
		return rangeIncl(ChunkPos(), ChunkPos(maxInclusive));
	}
	static ChunkPos3Iterator rangeIncl(const BlockPos& min, const BlockPos& maxInclusive) {
		return rangeIncl(ChunkPos(min), ChunkPos(maxInclusive));
	}
	static ChunkPos3Iterator rangeIncl(const BlockCuboid& bounds) {
		return rangeIncl(bounds.minInclusive, bounds.maxInclusive());
	}

	bool operator!=(const ChunkPos3Iterator& other) const {
		return mIdx != other.mIdx;
	}
	ChunkPos3Iterator& operator++() {
		++mIdx;

		++mPos.x;
		if (mPos.x >= mMax.x) {
			mPos.x = mMin.x;
			++mPos.z;
			if (mPos.z >= mMax.z) {
				mPos.z = mMin.z;
				++mPos.y;
			}
		}
		return *this;
	}

	const ChunkPos& operator*() {
		return mPos;
	}

	ChunkPos3Iterator begin() const {
		return ChunkPos3Iterator(mMin, mMax);
	}
	ChunkPos3Iterator end() const {
		const auto dim = mMax - mMin;
		const uint64_t endIndex = dim.x * dim.y * dim.z;
		return ChunkPos3Iterator(mMin, mMax, endIndex);
	}

private:
	const ChunkPos mMin;
	const ChunkPos mMax;
	ChunkPos mPos;
	uint64_t mIdx;
};
