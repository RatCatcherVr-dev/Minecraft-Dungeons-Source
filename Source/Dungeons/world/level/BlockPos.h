#pragma once
#include "common_header.h"
#include "CommonTypes.h"
#include "network/BinaryStream.h"

class Vec3;
class ChunkPos;
class AActor;

class BlockPos {
public:

	static const BlockPos MIN, MAX;
	static const BlockPos ONE, ZERO;
	static BlockPos max(const BlockPos& a, const BlockPos& b) {
		return BlockPos(
			std::max(a.x, b.x),
			std::max(a.y, b.y),
			std::max(a.z, b.z));
	}

	static BlockPos min(const BlockPos& a, const BlockPos& b) {
		return BlockPos(
			std::min(a.x, b.x),
			std::min(a.y, b.y),
			std::min(a.z, b.z));
	}

	int x, y, z;

	explicit BlockPos(int a) :
		x(a)
		, y(a)
		, z(a) {

	}

	BlockPos() :
		BlockPos(0) {

	}

	BlockPos(int x, int y, int z)
		: x(x)
		, y(y)
		, z(z){
	}

	BlockPos(const BlockPos& rhs)
		: x(rhs.x)
		, y(rhs.y)
		, z(rhs.z){
	}

	BlockPos(const Vec3&);

	BlockPos(const AActor&);

	explicit BlockPos(const FVector& v)
		: BlockPos(v.X, v.Y, v.Z) {
	}

	BlockPos(const ChunkPos& cp, int y = 0);

	BlockPos(float x, float y, float z)
		: x((int) floorf(x))
		, y((int) floorf(y))
		, z((int) floorf(z)) {
	}

	void set(int _x, int _y, int _z) {
		x = _x;
		y = _y;
		z = _z;
	}

	void set(const BlockPos& pos) {
		x = pos.x;
		y = pos.y;
		z = pos.z;
	}

	BlockPos& operator=(const BlockPos& rhs) {
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		return *this;
	}

	BlockPos operator*(int s) const {
		return BlockPos(x * s, y * s, z * s);
	}

	const BlockPos& operator*=(int s) {
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}

	BlockPos operator/(int s) const {
		return BlockPos(x / s, y / s, z / s);
	}

	BlockPos operator+(const BlockPos& rhs) const {
		return BlockPos(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	BlockPos operator-(const BlockPos& rhs) const {
		return BlockPos(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	BlockPos operator+(int s) const {
		return BlockPos(x + s, y + s, z + s);
	}

	BlockPos operator-(int s) const {
		return BlockPos(x - s, y - s, z - s);
	}

	const BlockPos& operator +=(const BlockPos& o) {
		x += o.x;
		y += o.y;
		z += o.z;
		return *this;
	}

	const BlockPos& operator -=(const BlockPos& o) {
		x -= o.x;
		y -= o.y;
		z -= o.z;
		return *this;
	}

	BlockPos operator-() const {
		return BlockPos(-x, -y, -z);
	}

	const int& operator[](int index) const {
		switch (index) {
			case 0: return x;
			case 1: return y;
			case 2: return z;
				// Out of bounds
			default: DEBUG_ASSERT(false, "Invalid index for BlockPos, valid is [0,2]");
				return x;
		}
	}

	int& operator[](int index) {
		switch (index) {
			case 0: return x;
			case 1: return y;
			case 2: return z;
				// Out of bounds
			default: DEBUG_ASSERT(false, "Invalid index for BlockPos, valid is [0,2]");
				return x;
		}
	}

	BlockPos offset(int _x, int _y, int _z) const {
		return BlockPos(x + _x, y + _y, z + _z);
	}

	BlockPos above() const {
		return BlockPos(x, y + 1, z);
	}

	BlockPos above(int steps) const {
		return BlockPos(x, y + steps, z);
	}

	BlockPos below() const {
		return BlockPos(x, y - 1, z);
	}

	BlockPos below(int steps) const {
		return BlockPos(x, y - steps, z);
	}

	BlockPos north() const {
		return BlockPos(x, y, z - 1);
	}

	BlockPos north(int steps) const {
		return BlockPos(x, y, z - steps);
	}

	BlockPos south() const {
		return BlockPos(x, y, z + 1);
	}

	BlockPos south(int steps) const {
		return BlockPos(x, y, z + steps);
	}

	BlockPos west() const {
		return BlockPos(x - 1, y, z);
	}

	BlockPos west(int steps) const {
		return BlockPos(x - steps, y, z);
	}

	BlockPos east() const {
		return BlockPos(x + 1, y, z);
	}

	BlockPos east(int steps) const {
		return BlockPos(x + steps, y, z);
	}

	BlockPos neighbor(FacingID direction) const;

	Vec3 center() const;

	void move(int _x, int _y, int _z) {
		x += _x;
		y += _y;
		z += _z;
	}

	void move(const BlockPos& pos) {
		x += pos.x;
		y += pos.y;
		z += pos.z;
	}

	void moveX(int steps) {
		x += steps;
	}

	void moveY(int steps) {
		y += steps;
	}

	void moveZ(int steps) {
		z += steps;
	}

	void moveUp(int steps) {
		y += steps;
	}

	void moveUp() {
		++y;
	}

	void moveDown(int steps) {
		y -= steps;
	}

	void moveDown() {
		--y;
	}

	void moveEast(int steps) {
		x += steps;
	}

	void moveEast() {
		++x;
	}

	void moveWest(int steps) {
		x -= steps;
	}

	void moveWest() {
		--x;
	}

	void moveNorth(int steps) {
		z -= steps;
	}

	void moveNorth() {
		--z;
	}

	void moveSouth(int steps) {
		z += steps;
	}

	void moveSouth() {
		++z;
	}

	std::string toString() const {
		std::stringstream ss;
		ss << "Pos(" << x << "," << y << "," << z << ")";
		return ss.str();
	}

	float distSqr(const BlockPos& pos) const {
		float dx = (float)(x - pos.x);
		float dy = (float)(y - pos.y);
		float dz = (float)(z - pos.z);
		return dx * dx + dy * dy + dz * dz;
	}

	float distSqrToCenter(float x_, float y_, float z_) const {
		float dx = x + 0.5f - x_;
		float dy = y + 0.5f - y_;
		float dz = z + 0.5f - z_;
		return dx * dx + dy * dy + dz * dz;
	}

	bool operator==(const BlockPos& rhs) const {
		return x == rhs.x && y == rhs.y && z == rhs.z;
	}

	bool operator!=(const BlockPos& rhs) const {
		return x != rhs.x || y != rhs.y || z != rhs.z;
	}

	std::size_t hashCode() const;

	BlockPos toRenderChunkPosition() const {
		return BlockPos(x & 0xfffffff0, y & 0xfff0, z & 0xfffffff0);
	}

	BlockPos relative(FacingID facing, int steps = 1) const;

	int64_t product() const {
		return x * y * z;
	}

	int64_t productXz() const {
		return x * z;
	}

	int sum() const {
		return x + y + z;
	}

	int absSum() const {
		return std::abs(x) + std::abs(y) + std::abs(z);
	}

	int sumXz() const {
		return x + z;
	}

	int absSumXz() const {
		return std::abs(x) + std::abs(z);
	}

	bool isPositive() const {
		return x >= 0 && y >= 0 && z >= 0;
	}
};

class BlockPosIterator {
public:
	BlockPosIterator(const BlockPos& a, const BlockPos& b);

	bool operator!=(const BlockPosIterator& other) const;
	BlockPosIterator& operator++();
	const BlockPos& operator*();

	BlockPosIterator begin() const;
	BlockPosIterator end() const;

private:
	const BlockPos mMinCorner;
	const BlockPos mMaxCorner;
	BlockPos mCurrentPos;
	bool mDone;
};

namespace std {
	///hash specialization for unordered_maps
	template<>
	struct hash<BlockPos>{
		// hash functor for vector
		size_t operator()(const BlockPos& _Keyval) const {
			return _Keyval.hashCode();
		}

	};

}

struct BlockCuboid;

//
// Iterator
//
class BlockPosIteration {
	BlockPosIteration(const BlockPos& min, const BlockPos& max);
public:
	static BlockPosIteration range(const BlockPos& pos);
	static BlockPosIteration range(const BlockPos& min, const BlockPos& max);
	static BlockPosIteration range(const BlockCuboid&);
	static BlockPosIteration rangeIncl(const BlockPos& min, const BlockPos& maxIncl);

	struct Iterator : public BlockPos {
		Iterator(const BlockPos& min, const BlockPos& max, uint64_t startIdx = 0);

		Iterator& operator++();
		BlockPos& operator*();
		bool operator!=(const Iterator& rhs);

		inline uint64_t index() {
			return mIdx;
		}
	private:
		const BlockPos mMin;
		const BlockPos mMax;
		uint64_t mIdx;
	};

	Iterator begin() const;
	Iterator end() const;
private:
	const BlockPos mMin;
	const BlockPos mMax;
	const uint64_t mVolume;
};

using BlockPredicate = std::function<bool(BlockPos, FullBlock)>;
using BlockCallback = std::function<void(BlockPos, FullBlock)>;
using PosPredicate = std::function<bool(BlockPos)>;
using PosCallback = std::function<void(BlockPos)>;
using BlockPosTransform = std::function<BlockPos(const BlockPos&)>;
using BlockGetter = std::function<FullBlock(BlockPos)>;
using BlockSetter = BlockCallback;

struct BlockXzComparator {
	std::size_t operator()(BlockPos p) const { return p.x * 8976890 + p.z; }
	bool operator()(BlockPos a, BlockPos b) const { return a.x == b.x && a.z == b.z; }
};
