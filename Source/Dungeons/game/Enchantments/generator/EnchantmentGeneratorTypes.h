#pragma once

#include <algorithm>
#include "util/Math.h"

namespace game { namespace enchantment { namespace generator {

struct Counts {
	Counts(int rows, int minCountPerRow, int enchantments, int powerfulRows, int powerfulEnchantments)
		: rows(std::max(rows, 0))
		, minCountPerRow(std::max(minCountPerRow, 1))
		, enchantments(Math::clamp(enchantments, 0, rows * Columns))
		, powerfulRows(Math::clamp(powerfulRows, 0, rows))
		, powerfulEnchantments(Math::clamp(powerfulEnchantments, 0, std::min(enchantments, powerfulRows * Columns))) {
	}

	static constexpr int Columns = 3;
	int firstIndexOfRow(int row) const { return (row >= 0 ? row : rows + row) * Columns; } // python style negative indexing

	const int rows;
	const int minCountPerRow;
	const int enchantments;
	const int powerfulRows;
	const int powerfulEnchantments;
};

}}}

namespace enchgen = game::enchantment::generator;
