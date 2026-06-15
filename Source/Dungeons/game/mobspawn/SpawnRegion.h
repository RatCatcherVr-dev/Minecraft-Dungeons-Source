#pragma once
#include "lovika/Region.h"

class Random;

namespace game {
namespace mobspawn {

class Region {
public:
	Region(const lovika::Region* region);

	Vec3 randomPos(Random&, float radius = 0) const;

	BlockCuboid area() const;

	const std::string& name() const;

	bool isValid() const;

	explicit operator bool() const;
private:
	const lovika::Region* mRegion;
};

Vec3 randomPos(const BlockCuboid&, Random&, float radius = 0);
float randomRotation(const std::vector<float>& angles, Random&);
}}
