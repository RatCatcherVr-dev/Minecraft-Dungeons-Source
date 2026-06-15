#include "Dungeons.h"
#include "QuadrantAngle.h"

static const std::array<QuadrantAngle, 4> _quadrantAngles {
	QuadrantAngle::D0,
	QuadrantAngle::D90,
	QuadrantAngle::D180,
	QuadrantAngle::D270
};

QuadrantAngle quadrantFromDegrees(int degrees) {
	return _fromWrappedInt(Math::intFloorDiv(degrees + 45, 90));
}

QuadrantAngle quadrantFromRadians(float radians) {
	return quadrantFromDegrees(Math::floor(radians * Math::RADDEG));
}

int quadrantToDegrees(QuadrantAngle angle) {
	return 90 * static_cast<int>(angle);
}

float quadrantToRadians(QuadrantAngle angle) {
	return HALF_PI * static_cast<float>(angle);
}

QuadrantAngle quadrantFromDelta(BlockPos delta) {
	DEBUG_ASSERT(delta.x != 0 || delta.z != 0, "A zero vector has no direction");
	DEBUG_ASSERT(delta.x == 0 || delta.z == 0, "Only support orthogonal deltas");
	if (delta.z != 0) {
		return delta.z < 0 ? QuadrantAngle::D90 : QuadrantAngle::D270;
	} else {
		return delta.x < 0 ? QuadrantAngle::D180 : QuadrantAngle::D0;
	}
}

QuadrantAngle quadrantFromDelta(float dx, float dyz) {
	if (std::abs(dyz) > std::abs(dx)) {
		return dyz < 0 ? QuadrantAngle::D90 : QuadrantAngle::D270;
	} else {
		return dx < 0 ? QuadrantAngle::D180 : QuadrantAngle::D0;
	}
}

QuadrantAngle quadrantFromFacing(FacingID facing) {
	switch (facing) {
	case Facing::EAST: return QuadrantAngle::D0;
	case Facing::NORTH: return QuadrantAngle::D90;
	case Facing::WEST: return QuadrantAngle::D180;
	case Facing::SOUTH: return QuadrantAngle::D270;
	default:
		DEBUG_FAIL("Invalid FacingID for conversion to QuadrantAngle");
		return QuadrantAngle::D0;
	}
}

Facing::Name quadrantToFacing(QuadrantAngle angle) {
	switch (angle) {
	case QuadrantAngle::D0: return Facing::EAST;
	case QuadrantAngle::D90: return Facing::NORTH;
	case QuadrantAngle::D180: return Facing::WEST;
	case QuadrantAngle::D270: return Facing::SOUTH;
	default:
		DEBUG_FAIL("Invalid QuadrantAngle");
		return Facing::NOT_DEFINED;
	}
}

QuadrantAngle angleBetween(FacingID src, FacingID target) {
	return quadrantFromFacing(target) - quadrantFromFacing(src);
}

const std::array<QuadrantAngle, 4>& quadrantAngles() {
	return _quadrantAngles;
}

QuadrantAngle& operator++(QuadrantAngle& angle) {
	return angle = _fromWrappedInt(static_cast<int>(angle) + 1);
}

QuadrantAngle& operator--(QuadrantAngle& angle) {
	return angle = _fromWrappedInt(static_cast<int>(angle) - 1);
}

QuadrantAngle operator+(const QuadrantAngle& a, const QuadrantAngle& b) {
	return _fromWrappedInt(static_cast<int>(a) + static_cast<int>(b));
}

QuadrantAngle operator-(const QuadrantAngle& a, const QuadrantAngle& b) {
	return _fromWrappedInt(static_cast<int>(a) - static_cast<int>(b));
}

Facing::Name operator+(const FacingID& facing, const QuadrantAngle& rotation) {
	return quadrantToFacing(quadrantFromFacing(facing) + rotation);
}

bool isPerpendicularlyRotated(QuadrantAngle angle) {
	return angle == QuadrantAngle::D90 || angle == QuadrantAngle::D270;
}

QuadrantAngle opposite(QuadrantAngle angle) {
	return angle + QuadrantAngle::D180;
}
