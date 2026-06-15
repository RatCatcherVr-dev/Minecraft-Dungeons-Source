#pragma once

#include "CommonTypes.h"
#include <Color.h>

class RegionType {
public:
	RegionType(int typeId, std::string name, FColor color);
	RegionType(RegionType&&) = default;
	RegionType(const RegionType&) = delete;
	RegionType& operator=(const RegionType&) = delete;

	std::string displayName;
	std::string name;
	FColor color;

	bool operator==(const RegionType& rhs) const { return this == &rhs; }
	bool operator!=(const RegionType& rhs) const { return this != &rhs; }
private:
	int type;
};

namespace regiontype {

extern const RegionType& Prefab;
extern const RegionType& Spawn;
extern const RegionType& PropArea;
extern const RegionType& Trigger;
extern const RegionType& Loot;
extern const RegionType& Door;

// For use in the editor and/or game -> editor serialization
extern const RegionType& PlacedPrefab;
extern const RegionType& PrefabInstanceClone;

const RegionType* fromString(const std::string&);

}
