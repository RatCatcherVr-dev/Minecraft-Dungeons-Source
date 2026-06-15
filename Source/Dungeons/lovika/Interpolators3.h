#pragma once
#include "world/phys/Vec3.h"

template <typename value_type>
class InterpolatorXZ_3 {
	template <typename vt>
	struct Edge {
		Edge(Vec3 v0, Vec3 v1, vt w)
			: xx(w * (v1.x - v0.x))
			, zz(w * (v0.z - v1.z))
			, s(w * (v0.x * v1.z - v0.z * v1.x)) {
		}

		vt operator() (Vec3 p) const { return zz * p.x + xx * p.z + s; }

		void scale(float k) {
			xx *= k;
			zz *= k;
			s *= k;
		}
	private:
		vt xx, zz, s;
	};
public:
	InterpolatorXZ_3(Vec3 a, value_type valueA, Vec3 b, value_type valueB, Vec3 c, value_type valueC)
		: me01(a, b, valueC)
		, me12(b, c, valueA)
		, me20(c, a, valueB) {
		const float area = 1.0f / Edge<float>(a, b, 1)(c);
		me01.scale(area);
		me12.scale(area);
		me20.scale(area);
	}

	value_type operator()(Vec3 p) const {
		return me01(p) + me12(p) + me20(p);
	}

	bool isInside(Vec3 p) const {
		return me01(p) >= 0 && me12(p) >= 0 && me20(p) >= 0;
	}

	bool isInside(Vec3 p, float margin) const {
		float w0 = me12(p), w1 = me20(p), w2 = me01(p);
		return std::min(w0, std::min(w1, w2)) >= -margin && (w0 + w1 + w2 <= 1 + margin);
	}
private:
	Edge<value_type> me01, me12, me20;
};
