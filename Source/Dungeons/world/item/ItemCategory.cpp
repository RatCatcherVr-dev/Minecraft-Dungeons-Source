#include "Dungeons.h"

#include "world/item/ItemCategory.h"

CreativeItemCategory CreativeItemCategoryFromString(const std::string& str) {
	if (str.empty()) {
		return CreativeItemCategory::All;
	}

	// Lazy Init of static lookup map
	static std::unordered_map<std::string, CreativeItemCategory> ENUM_MAP = {
		{ "all", CreativeItemCategory::All },
		{ "buildingblocks", CreativeItemCategory::BuildingBlocks },
		{ "decorations", CreativeItemCategory::Decorations },
		{ "tools", CreativeItemCategory::Tools },
		{ "miscellaneous", CreativeItemCategory::Miscellaneous },
		{ "none", CreativeItemCategory::None }
	};

	// Lookup case insensitive by using *lower case* keys.
	auto i = ENUM_MAP.find(Util::toLower(str));
	if (i != ENUM_MAP.end()) {
		return i->second;
	}

	return CreativeItemCategory::All;
}
