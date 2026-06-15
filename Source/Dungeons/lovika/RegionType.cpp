#include "Dungeons.h"
#include "RegionType.h"
#include "legacy/Core/Utility/EnumCast.h"
#include "util/CollectionUtils.h"

RegionType::RegionType(int typeId, std::string name, FColor color)
	: type(typeId)
	, displayName(name)
	, name(Util::toLower(name))
	, color(std::move(color))
{}

namespace regiontype {

TArray<RegionType> allRegionDefs = Util::createTArrayOfReservedSize<RegionType>(20);

const RegionType& _register(RegionType region) {
	allRegionDefs.Add(std::move(region));
	return allRegionDefs.Top();
}

FColor color(float r, float g, float b, float a) {
	return FLinearColor(r, g, b, a).ToFColor(true);
}

constexpr float a = 0.05f;

const RegionType& Prefab    = _register(RegionType(1, "prefab", color(0.2f, 1, 1, a)));
const RegionType& Spawn     = _register(RegionType(2, "spawn", color(1, 0, 0, a)));
const RegionType& Door      = _register(RegionType(3, "door", color(0.7f, 0.9f, 0.7f, a)));
const RegionType& Trigger   = _register(RegionType(4, "trigger", color(0.25f, 0.5f, 1.0f, a)));
const RegionType& Loot      = _register(RegionType(5, "loot", color(0.5f, 0.2f, 1.0f, a)));
const RegionType& PropArea  = _register(RegionType(6, "prop-area", color(1, 0, 1, a)));
// not user placable
const RegionType& PlacedPrefab	    = _register(RegionType(7, "placed-prefab", color(0.5f, 1, 0.6f, a)));
const RegionType& PrefabInstanceClone = _register(RegionType(8, "instance-clone", color(0, 1, 1, a)));
// unknown
const RegionType& unknown   = _register(RegionType(0, "unknown", color(1, 1, 1, 1)));


const RegionType* fromString(const std::string& name) {
	const auto id = Util::toLower(name);

	for (const auto& def : allRegionDefs) {
		if (id == def.name) {
			return &def;
		}
	}
	return nullptr;
}

}
