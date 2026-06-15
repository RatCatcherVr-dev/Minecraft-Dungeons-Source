#include "Dungeons.h"
#include "DirectionMask.h"

const DirectionMask DirectionMask::maskValueOf(Facing::Name facing) {
	return DirectionMask(1 << facing);
}

const bool DirectionMask::isInSamePlane(const BlockPos& a, const BlockPos& b, Facing::Name faceNormal) {
	if (Facing::getStepX(faceNormal)) {
		return a.x == b.x;
	}
	if (Facing::getStepY(faceNormal)) {
		return a.y == b.y;
	}
	if (Facing::getStepZ(faceNormal)) {
		return a.z == b.z;
	}
	return false;
}

const DirectionMask DirectionMask::X_POS = maskValueOf(Facing::EAST);
const DirectionMask DirectionMask::X_NEG = maskValueOf(Facing::WEST);
const DirectionMask DirectionMask::Y_POS = maskValueOf(Facing::UP);
const DirectionMask DirectionMask::Y_NEG = maskValueOf(Facing::DOWN);
const DirectionMask DirectionMask::Z_POS = maskValueOf(Facing::SOUTH);
const DirectionMask DirectionMask::Z_NEG = maskValueOf(Facing::NORTH);

const DirectionMask DirectionMask::X = DirectionMask(X_POS | X_NEG);
const DirectionMask DirectionMask::Y = DirectionMask(Y_POS | Y_NEG);
const DirectionMask DirectionMask::Z = DirectionMask(Z_POS | Z_NEG);

const DirectionMask DirectionMask::XY = DirectionMask(X | Y);
const DirectionMask DirectionMask::XZ = DirectionMask(X | Z);
const DirectionMask DirectionMask::YZ = DirectionMask(Y | Z);
const DirectionMask DirectionMask::ALL = DirectionMask(X | Y | Z);

const DirectionMask DirectionMask::PLANE_FACING[6] = {
	DirectionMask::XZ,
	DirectionMask::XZ,
	DirectionMask::XY,
	DirectionMask::XY,
	DirectionMask::YZ,
	DirectionMask::YZ
};
