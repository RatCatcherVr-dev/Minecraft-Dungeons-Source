#pragma once

#include "WalkableHeight.h"
#include "lovika/BlockCuboid.h"
#include "lovika/BlockRegion.h"
#include "lovika/RegionType.h"
#include "LevelFileCommonTypes.h"

namespace io {

struct Region {
	std::string name;
	BlockCuboid bounds;
	const RegionType& type;
	std::string tags;

	bool operator==(const Region& rhs) const {
		return name == rhs.name && bounds == rhs.bounds && type == rhs.type && tags == rhs.tags;
	}
};

struct Door {
	std::string name;
	BlockCuboid bounds;
	std::string tags;

	bool operator==(const Door& rhs) const {
		return name == rhs.name && bounds == rhs.bounds && tags == rhs.tags;
	}
};

struct Boundary {
	//D11.PS - Had to remove the const for PS4 D11.TODO - look further into this
	/*const */BlockPos position;
	/*const */int height;

	bool operator==(const Boundary& rhs) const {
		return position == rhs.position && height == rhs.height;
	}
};

struct Object {
	std::string id;
	BlockCuboid bounds;
	int y;
	std::string tags;
	std::vector<Door> doors;
	std::vector<Region> regions;
	std::vector<Boundary> boundaries;
	std::vector<uint8_t> regionPlane;
	std::vector<uint8_t> heightPlane;
	std::vector<uint8_t> maxHeightOfRegionTerrainPlane;
	std::vector<WalkableHeight> walkablePlane;
	std::vector<WalkableHeight> walkablePlaneFromLovika;
	
	Unique<BlockRegion> blocks;

	bool isTile() const {
		return !doors.empty();
	}
};

struct ObjectGroup {
	ObjectGroupDef def;
	std::vector<Object> objects;
};


Unique<ObjectGroup> loadObjectGroup(std::istream&, const std::string& path);
Unique<ObjectGroup> loadObjectGroup(TArray<uint8>&, const std::string& path);
void saveObjectGroup(const ObjectGroup&, std::ostream&);

};
