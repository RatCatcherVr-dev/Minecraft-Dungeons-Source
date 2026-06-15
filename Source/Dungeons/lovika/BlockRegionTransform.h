#pragma once

#include "BlockRegion.h"

using RegionSize = BlockRegion::RegionSize;
using RegionPos = BlockRegion::RegionPos;
enum class QuadrantAngle :int;

class BlockRegionTransform {
public:
	static const BlockRegionTransform& Identity();
	static const BlockRegionTransform& Rotate90();
	static const BlockRegionTransform& Rotate180();
	static const BlockRegionTransform& Rotate270();
	static const BlockRegionTransform& FlipX();
	static const BlockRegionTransform& FlipZ();
	static const BlockRegionTransform& GetRotate(QuadrantAngle);

	virtual ~BlockRegionTransform() {}
	virtual RegionSize transformSize(const RegionSize&) const = 0;
	virtual RegionPos transformPos(const RegionPos&, const RegionSize&) const = 0;
};
