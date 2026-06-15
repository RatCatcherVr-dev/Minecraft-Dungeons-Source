#pragma once
#include "world/phys/Vec2.h"
#include "world/phys/Vec3.h"

template <typename T>
T lerp(T v0, T v1, float t) {
	return (1 - t) * v0 + t * v1;
}

template <typename value_type>
class InterpolatorXZ_2 {
public:
	InterpolatorXZ_2(Vec3 a, value_type valueA, Vec3 b, value_type valueB)
		: InterpolatorXZ_2(Vec2(a.x, a.z), valueA, Vec2(b.x, b.z), valueB) {
	}

	InterpolatorXZ_2(Vec2 a, value_type valueA, Vec2 b, value_type valueB)
		: mPos(a)
		, mDir((b - mPos).normalized())
		, mInv(1.0f / (b - mPos).length())
		, mVa(valueA)
		, mVb(valueB) {
	}

	value_type operator()(Vec2 p) const {
		auto q = p - mPos;
		return lerp(mVa, mVb, mInv * q.dot(mDir));
	}

	value_type operator()(Vec3 p) const {
		auto q = Vec2(p.x, p.z) - mPos;
		return lerp(mVa, mVb, mInv * q.dot(mDir));
	}
private:
	Vec2 mPos, mDir;
	float mInv;
	value_type mVa, mVb;
};
