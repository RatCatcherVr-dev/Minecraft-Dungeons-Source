#pragma once
#include "SpawnRegion.h"

namespace game {
namespace mobspawn {

class Regions {
public:
	Regions() {}
	Regions(const Regions&) = default;
	Regions(std::vector<lovika::Region>);

	bool isEmpty() const;
	int count() const;
	int totalArea() const;

	Region get(size_t index) const;
	Region getRandom(Random&) const;
	Vec3   getRandomPosFromRandomRegion(float radius = 0.25f, Random* = nullptr) const;

	explicit operator bool() const;
private:
	static const Region INVALID;
	std::vector<lovika::Region> mRegions;
};

}}
