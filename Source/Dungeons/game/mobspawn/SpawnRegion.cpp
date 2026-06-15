#include "Dungeons.h"
#include "SpawnRegion.h"
#include "util/Random.h"

namespace game { namespace mobspawn {

Region::Region(const lovika::Region* region)
	: mRegion(region) {
}

Vec3 Region::randomPos(Random& rnd, float radius /*= 0 */) const {
	return mobspawn::randomPos(mRegion->area(), rnd, radius);
}

BlockCuboid Region::area() const {
	return mRegion->area();
}

const std::string& Region::name() const {
	return mRegion->name();
}

bool Region::isValid() const {
	return mRegion != nullptr;
}

Region::operator bool() const {
	return isValid();
}

Vec3 randomPos(const BlockCuboid& area, Random& rnd, float radius /* = 0 */) {
	Vec3 min(area.minInclusive), max(area.maxExclusive);
	bool enoughWidth = (max.x - min.x) > radius + radius;
	auto minX = enoughWidth ? min.x + radius : 0.5f * (min.x + max.x);
	auto maxX = enoughWidth ? max.x - radius : minX;
	bool enoughDepth = (max.z - min.z) > radius + radius;
	auto minZ = enoughDepth ? min.z + radius : 0.5f * (min.z + max.z);
	auto maxZ = enoughDepth ? max.z - radius : minZ;
	return Vec3(rnd.nextFloat(minX, maxX), min.y, rnd.nextFloat(minZ, maxZ));
}

float randomRotation(const std::vector<float>& angles, Random& rnd) {
	return angles[rnd.nextUnsignedInt(angles.size())];
}
}}
