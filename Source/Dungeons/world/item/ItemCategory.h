#pragma once

enum class CreativeItemCategory : int
{
	All = 0,
	BuildingBlocks = 1,
	Decorations = 2,
	Tools = 3,
	Miscellaneous = 4,

	None = 5,

	NUM_CATEGORIES = 6,
};
CreativeItemCategory CreativeItemCategoryFromString(const std::string& str);
