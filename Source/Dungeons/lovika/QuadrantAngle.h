#pragma once
#include "world/Facing.h"

// Counter-clockwise angles
enum class QuadrantAngle: int {
	D0,
	D90,
	D180,
	D270
};

static inline QuadrantAngle _fromValidInt(int index) {
	return static_cast<QuadrantAngle>(index);
}

static inline QuadrantAngle _fromWrappedInt(int index) {
	return _fromValidInt(Math::mod(index, 4));
}

QuadrantAngle quadrantFromDegrees(int);

QuadrantAngle quadrantFromRadians(float);

int quadrantToDegrees(QuadrantAngle);

float quadrantToRadians(QuadrantAngle);

QuadrantAngle quadrantFromDelta(BlockPos delta);
QuadrantAngle quadrantFromDelta(float dx, float dyz);

QuadrantAngle quadrantFromFacing(FacingID);

Facing::Name quadrantToFacing(QuadrantAngle);

QuadrantAngle angleBetween(FacingID src, FacingID target);

const std::array<QuadrantAngle, 4>& quadrantAngles();

QuadrantAngle& operator++(QuadrantAngle&);

QuadrantAngle& operator--(QuadrantAngle&);

QuadrantAngle operator+(const QuadrantAngle&, const QuadrantAngle&);

QuadrantAngle operator-(const QuadrantAngle&, const QuadrantAngle&);

Facing::Name operator+(const FacingID&, const QuadrantAngle& rotation);

bool isPerpendicularlyRotated(QuadrantAngle);

QuadrantAngle opposite(QuadrantAngle);
