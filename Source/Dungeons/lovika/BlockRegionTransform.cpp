#include "Dungeons.h"
#include "BlockRegionTransform.h"
#include "QuadrantAngle.h"

static RegionSize IdentitySize(const RegionSize& size) { return size; }
static RegionPos  IdentityPos(const RegionPos& pos, const RegionSize& size) { return pos; }
static RegionSize SwapXZSize(const RegionSize& size) { return RegionSize(size.z, size.y, size.x); }
static RegionPos  FlipXPos(const RegionPos& pos, const RegionSize& size) { return RegionPos(size.x - 1 - pos.x, pos.y, pos.z); }
static RegionPos  FlipZPos(const RegionPos& pos, const RegionSize& size) { return RegionPos(pos.x, pos.y, size.z - 1 - pos.z); }
static RegionPos  Rotate90Pos(const RegionPos& pos, const RegionSize& size) { return RegionPos(pos.z, pos.y, size.z - 1 - pos.x); }
static RegionPos  Rotate180Pos(const RegionPos& pos, const RegionSize& size) { return RegionPos(size.x - 1 - pos.x, pos.y, size.z - 1 - pos.z); }
static RegionPos  Rotate270Pos(const RegionPos& pos, const RegionSize& size) { return RegionPos(size.x - 1 - pos.z, pos.y, pos.x); }

class BlockRegionFunctionTransform : public BlockRegionTransform {
	using SizeFunction = std::function<RegionSize(RegionSize)>;
	using PosFunction = std::function<RegionPos(RegionPos, RegionSize)>;
public:
	BlockRegionFunctionTransform(SizeFunction, PosFunction);

	RegionSize transformSize(const RegionSize&) const override;
	RegionPos transformPos(const RegionPos&, const RegionSize&) const override;
private:
	SizeFunction mSize;
	PosFunction mPos;
};

static const BlockRegionFunctionTransform
sIdentity(IdentitySize, IdentityPos),
sRotate90(SwapXZSize, Rotate90Pos),
sRotate180(IdentitySize, Rotate180Pos),
sRotate270(SwapXZSize, Rotate270Pos),
sFlipX(IdentitySize, FlipXPos),
sFlipZ(IdentitySize, FlipZPos);

const BlockRegionTransform& BlockRegionTransform::Identity() {
	return sIdentity;
}

const BlockRegionTransform& BlockRegionTransform::Rotate90() {
	return sRotate90;
}

const BlockRegionTransform& BlockRegionTransform::Rotate180() {
	return sRotate180;
}

const BlockRegionTransform& BlockRegionTransform::Rotate270() {
	return sRotate270;
}

const BlockRegionTransform& BlockRegionTransform::FlipX() {
	return sFlipX;
}

const BlockRegionTransform& BlockRegionTransform::FlipZ() {
	return sFlipZ;
}

const BlockRegionTransform& BlockRegionTransform::GetRotate(QuadrantAngle angle) {
	switch (angle) {
	case QuadrantAngle::D90:  return sRotate90;
	case QuadrantAngle::D180: return sRotate180;
	case QuadrantAngle::D270: return sRotate270;
	default:
		return sIdentity;
	}
}

BlockRegionFunctionTransform::BlockRegionFunctionTransform(SizeFunction size, PosFunction pos)
	: mSize(size)
	, mPos(pos) {
}

RegionSize BlockRegionFunctionTransform::transformSize(const RegionSize& size) const {
	return mSize(size);
}

RegionPos BlockRegionFunctionTransform::transformPos(const RegionPos& pos, const RegionSize& size) const {
	return mPos(pos, size);
}
