#include "Dungeons.h"
#include "BlockPosTransform.h"
#include "QuadrantAngle.h"

BlockPosTransform blockpostransform::offset(BlockPos offset) {
	return [offset](const BlockPos& pos) {
		return pos + offset;
	};
}

BlockPosTransform blockpostransform::create(const RegionSize& size, const BlockRegionTransform& transform) {
	auto newSize = transform.transformSize(size);
	return [&transform, newSize](const BlockPos& pos) {
		return transform.transformPos(pos, newSize);
	};
}

BlockPosTransform blockpostransform::create(const RegionSize& size, const BlockRegionTransform& transform, const BlockPos& offset) {
	auto newSize = transform.transformSize(size);
	return [&transform, newSize, offset](const BlockPos& pos) {
		return transform.transformPos(pos, newSize) + offset;
	};
}

BlockPosTransform blockpostransform::_invert(const BlockPosTransform& transform) {
	auto o = transform(BlockPos(0, 0, 0));

	switch (quadrantFromDelta(transform(BlockPos(1, 0, 0)) - o)) {
	case QuadrantAngle::D90: return [=](auto p) { return BlockPos(o.z - p.z, p.y - o.y, p.x - o.x); };
	case QuadrantAngle::D180: return [=](auto p) { return BlockPos(o.x - p.x, p.y - o.y, o.z - p.z); };
	case QuadrantAngle::D270: return [=](auto p) { return BlockPos(p.z - o.z, p.y - o.y, o.x - p.x); };
	default: return [=](auto p) { return p - o; };
	}
}

QuadrantAngle blockpostransform::rotation(const BlockPosTransform& transform) {
	return quadrantFromDelta(transform(BlockPos(1, 0, 0)) - transform(BlockPos::ZERO));
}
