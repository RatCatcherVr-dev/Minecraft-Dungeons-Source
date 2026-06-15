#include "Dungeons.h"
#include "BlockCuboid.h"
#include "world/phys/Vec3.h"
#include "game/Conversion.h"

static bool lessThanOrEquals(BlockPos a, BlockPos b) {
	return a.x <= b.x && a.y <= b.y && a.z <= b.z;
}

BlockCuboid::BlockCuboid(const BlockPos& minInclusive, const BlockPos& maxExclusive)
	: minInclusive(minInclusive)
	, maxExclusive(maxExclusive)
{
}

BlockCuboid BlockCuboid::fromPositionAndSize(const BlockPos& minInclusive, int sizeX, int sizeY, int sizeZ) {
	return BlockCuboid(minInclusive, minInclusive.offset(sizeX, sizeY, sizeZ));
}

BlockCuboid BlockCuboid::fromPositionAndSize(const BlockPos& minInclusive, const BlockPos& size) {
	return BlockCuboid(minInclusive, minInclusive + size);
}

BlockCuboid BlockCuboid::fromInclusiveCorners(const BlockPos& a, const BlockPos& b) {
	return BlockCuboid(BlockPos::min(a, b), BlockPos::max(a, b) + 1);
}

BlockCuboid BlockCuboid::fromSize(const BlockPos& size) {
	return BlockCuboid(BlockPos(), size);
}

BlockPos BlockCuboid::size() const {
	return maxExclusive - minInclusive;
}

unsigned int BlockCuboid::area() const {
	return std::abs((maxExclusive.x - minInclusive.x) * (maxExclusive.z - minInclusive.z));
}

bool BlockCuboid::isEmpty() const {
	return (minInclusive.x == maxExclusive.x) || (minInclusive.y == maxExclusive.y) || (minInclusive.z == maxExclusive.z);
}

uint64_t BlockCuboid::volume() const {
	return std::abs(size().product());
}

bool BlockCuboid::fits(unsigned int sizeX, unsigned int sizeY, unsigned int sizeZ) const {
	return lessThanOrEquals(minInclusive + BlockPos((int)sizeX, (int)sizeY, (int)sizeZ), maxExclusive);
}

bool BlockCuboid::fits(BlockPos size) const {
	DEBUG_ASSERT(size.isPositive(), "Size must be >= 0");
	return lessThanOrEquals(minInclusive + size, maxExclusive);
}

bool BlockCuboid::fitsXZ(unsigned int sizeX, unsigned int sizeZ) const {
	return minInclusive.x + (int)sizeX <= maxExclusive.x && minInclusive.z + (int)sizeZ <= maxExclusive.z;
}

bool BlockCuboid::fitsXZ(BlockPos size) const {
	DEBUG_ASSERT(size.x >= 0 && size.z >= 0, "Size must be >= 0");
	return minInclusive.x + size.x <= maxExclusive.x && minInclusive.z + size.z <= maxExclusive.z;
}

void BlockCuboid::expand(const BlockPos& include) {
	minInclusive = BlockPos::min(minInclusive, include);
	maxExclusive = BlockPos::max(maxExclusive, include + 1);
}

void BlockCuboid::expand(const BlockCuboid& include) {
	expand(include.minInclusive);
	expand(include.maxInclusive());
}

bool BlockCuboid::containsX(int x) const {
	return x >= minInclusive.x && x < maxExclusive.x;
}

bool BlockCuboid::containsY(int y) const {
	return y >= minInclusive.y && y < maxExclusive.y;
}

bool BlockCuboid::containsZ(int z) const {
	return z >= minInclusive.z && z < maxExclusive.z;
}

bool BlockCuboid::contains(const BlockPos& pos) const {
	return pos.x >= minInclusive.x && pos.x < maxExclusive.x
		&& pos.y >= minInclusive.y && pos.y < maxExclusive.y
		&& pos.z >= minInclusive.z && pos.z < maxExclusive.z;
}

bool BlockCuboid::containsXZ(const BlockPos& pos) const {
	return pos.x >= minInclusive.x && pos.x < maxExclusive.x && pos.z >= minInclusive.z && pos.z < maxExclusive.z;
}

bool BlockCuboid::containsXZ(int x, int z) const {
	return x >= minInclusive.x && x < maxExclusive.x && z >= minInclusive.z && z < maxExclusive.z;
}

bool BlockCuboid::containsXZ(const BlockCuboid& other) const {
	return other.minInclusive.x >= minInclusive.x && other.maxExclusive.x <= maxExclusive.x
		&& other.minInclusive.z >= minInclusive.z && other.maxExclusive.z <= maxExclusive.z;
}

bool BlockCuboid::contains(const BlockCuboid& other) const {
	return other.minInclusive.x >= minInclusive.x && other.maxExclusive.x <= maxExclusive.x
		&& other.minInclusive.y >= minInclusive.y && other.maxExclusive.y <= maxExclusive.y
		&& other.minInclusive.z >= minInclusive.z && other.maxExclusive.z <= maxExclusive.z;
}

bool BlockCuboid::intersects(const BlockCuboid& other) const {
	return !(maxExclusive.x <= other.minInclusive.x || minInclusive.x >= other.maxExclusive.x
	      || maxExclusive.z <= other.minInclusive.z || minInclusive.z >= other.maxExclusive.z
	      || maxExclusive.y <= other.minInclusive.y || minInclusive.y >= other.maxExclusive.y);
}

bool BlockCuboid::intersectsXZ(const BlockCuboid& other) const {
	return !(maxExclusive.x <= other.minInclusive.x || minInclusive.x >= other.maxExclusive.x
	      || maxExclusive.z <= other.minInclusive.z || minInclusive.z >= other.maxExclusive.z);
}

BlockCuboid BlockCuboid::intersection(const BlockCuboid& other) const {
	BlockPos min = BlockPos::max(minInclusive, other.minInclusive);
	BlockPos max = BlockPos::min(maxExclusive, other.maxExclusive);
	return lessThanOrEquals(min, max) ? BlockCuboid(min, max) : BlockCuboid();
}

BlockPosIteration::Iterator BlockCuboid::begin() const {
	return BlockPosIteration::Iterator(minInclusive, maxExclusive, 0);
}

BlockPosIteration::Iterator BlockCuboid::end() const {
	return BlockPosIteration::Iterator(minInclusive, maxExclusive, volume());
}

BlockCuboid BlockCuboid::operator+(const BlockPos& offset) const {
	return BlockCuboid(minInclusive + offset, maxExclusive + offset);
}

BlockCuboid BlockCuboid::operator-(const BlockPos& offset) const {
	return BlockCuboid(minInclusive - offset, maxExclusive - offset);
}

BlockCuboid& BlockCuboid::operator+=(const BlockPos& offset) {
	minInclusive += offset;
	maxExclusive += offset;
	return *this;
}

BlockCuboid& BlockCuboid::operator-=(const BlockPos& offset) {
	minInclusive -= offset;
	maxExclusive -= offset;
	return *this;
}

BlockCuboid::operator FBox() const {
	return conversion::blockCuboidToUe(*this);
}

std::string BlockCuboid::toString() const {
	std::stringstream ss;
	ss << "BlockCuboid((" << minInclusive.x << "," << minInclusive.y << ","  << minInclusive.z << "), (" << maxExclusive.x << "," << maxExclusive.y << "," << maxExclusive.z << "))";
	return ss.str();
}

bool BlockCuboid::operator==(const BlockCuboid& rhs) const {
	return minInclusive == rhs.minInclusive && maxExclusive == rhs.maxExclusive;
}

bool BlockCuboid::operator!=(const BlockCuboid& rhs) const {
	return !(rhs == *this);
}

BlockCuboid squareCuboidFromCenter(BlockPos center, int radiusXZ) {
	return cuboidFromCenter(center, radiusXZ, LEVEL_HEIGHT_DEPRECATED);
}

BlockCuboid cuboidFromCenter(BlockPos center, int radiusXYZ) {
	return cuboidFromCenter(center, radiusXYZ, radiusXYZ);
}

BlockCuboid cuboidFromCenter(BlockPos center, int radiusXZ, int radiusY) {
	radiusXZ--;
	radiusY--;
	return BlockCuboid(
		BlockPos(center.x - radiusXZ, std::max(0, center.y - radiusY), center.z - radiusXZ),
		BlockPos(1 + center.x + radiusXZ, std::min((int) LEVEL_HEIGHT_DEPRECATED, 1 + center.y + radiusY), 1 + center.z + radiusXZ)
	);
}

BlockCuboid transformed(const BlockCuboid& volume, const BlockPosTransform& transform) {
	return BlockCuboid::fromInclusiveCorners(transform(volume.minInclusive), transform(volume.maxInclusive()));
}

void safeExpand(BlockCuboid& volumeOrEmpty, const BlockCuboid& include) {
	if (volumeOrEmpty.isEmpty()) {
		volumeOrEmpty = include;
	} else {
		volumeOrEmpty.expand(include);
	}
}

BlockCuboid flatten(const BlockCuboid& volume) {
	return flatten(volume, volume.minInclusive.y);
}

BlockCuboid flatten(const BlockCuboid& volume, int y) {
	return BlockCuboid(BlockPos(volume.minInclusive.x, y, volume.minInclusive.z), BlockPos(volume.maxExclusive.x, y + 1, volume.maxExclusive.z));
}

BlockCuboid flattenToHeight(const BlockCuboid& volume, int height) {
	return flattenToHeight(volume, volume.minInclusive.y, height);
}

BlockCuboid flattenToHeight(const BlockCuboid& volume, int y, int height) {
	return BlockCuboid(BlockPos(volume.minInclusive.x, y, volume.minInclusive.z), BlockPos(volume.maxExclusive.x, y + height, volume.maxExclusive.z));
}

Vec3 center(const BlockCuboid& volume, float lerpAlphaY) {
	return Vec3(
		(volume.minInclusive.x + volume.maxExclusive.x) * 0.5f,
		(volume.minInclusive.y * (1 - lerpAlphaY) + volume.maxExclusive.y * lerpAlphaY),
		(volume.minInclusive.z + volume.maxExclusive.z) * 0.5f
	);
}

Vec3 centerFloor(const BlockCuboid& volume) {
	return center(volume, 0.0f);
}

BlockCuboid growed(const BlockCuboid& volume, BlockPos offset) {
	return BlockCuboid(volume.minInclusive - offset, volume.maxExclusive + offset);
}

BlockCuboid shrinked(const BlockCuboid& volume, BlockPos offset) {
	return BlockCuboid(volume.minInclusive + offset, volume.maxExclusive - offset);
}

BlockPos signedDistanceTo(const BlockCuboid& volume, BlockPos p) {
	const auto minOffset = p - volume.minInclusive;
	const auto maxOffset = p - volume.maxInclusive();
	const auto absMinSigned = [](auto a, auto b) { return FMath::Abs(a) < FMath::Abs(b) ? a : b; };

	return BlockPos(
		volume.containsX(p.x) ? 0 : absMinSigned(minOffset.x, maxOffset.x),
		volume.containsY(p.y) ? 0 : absMinSigned(minOffset.y, maxOffset.y),
		volume.containsZ(p.z) ? 0 : absMinSigned(minOffset.z, maxOffset.z)
	);
}

int manhattanDistanceTo(const BlockCuboid& volume, BlockPos p) {
	return signedDistanceTo(volume, p).absSum();
}

int manhattanXzDistanceTo(const BlockCuboid& volume, BlockPos p) {
	return signedDistanceTo(volume, p).absSumXz();
}
