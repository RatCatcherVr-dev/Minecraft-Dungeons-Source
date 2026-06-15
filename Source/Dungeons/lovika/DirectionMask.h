#pragma once

#include "world/Facing.h"

struct DirectionMask: NewType<int> {
	static const DirectionMask maskValueOf(Facing::Name facing);

	static const bool isInSamePlane(const BlockPos& a, const BlockPos& b, Facing::Name faceNormal);

	static const DirectionMask X_POS;
	static const DirectionMask X_NEG;
	static const DirectionMask Y_POS;
	static const DirectionMask Y_NEG;
	static const DirectionMask Z_POS;
	static const DirectionMask Z_NEG;

    static const DirectionMask X;
	static const DirectionMask Y;
    static const DirectionMask Z;

	static const DirectionMask XY;
	static const DirectionMask XZ;
	static const DirectionMask YZ;
	static const DirectionMask ALL;
	static const DirectionMask PLANE_FACING[6];

	explicit DirectionMask(const int& mask) :
		NewType(mask) {
	}
};
