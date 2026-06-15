#include "Dungeons.h"
#include "SpawnRegions.h"
#include "util/SharedRandom.h"
#include "util/Algo.h"

namespace game { namespace mobspawn {

Regions::Regions(std::vector<lovika::Region> regions)
	: mRegions(std::move(regions)) {
}

const Region Regions::INVALID(nullptr);

int Regions::count() const {
	return (int) mRegions.size();
}

bool Regions::isEmpty() const {
	return mRegions.empty();
}

int Regions::totalArea() const {
	return algo::sum(mRegions, RETLAMBDA(it.area().area()));
}

Region Regions::get(size_t index) const {
	return Region(&mRegions[index]);
}

Region Regions::getRandom(Random& rnd) const {
	return isEmpty()? INVALID : get(rnd.nextInt(count()));
}

Vec3 Regions::getRandomPosFromRandomRegion(float radius /* = 0.25f*/, Random* rnd /* = nullptr*/) const {
	rnd = Util::thisOrSharedRandom(rnd);
	return isEmpty() ? Vec3{} : getRandom(*rnd).randomPos(*rnd, radius);
}

Regions::operator bool() const {
	return !isEmpty();
}

}}
