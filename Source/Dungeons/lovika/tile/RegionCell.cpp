#include "Dungeons.h"
#include "RegionCell.h"

RegionCell::RegionCell(const BlockPos& position, uint8_t value)
	: position(position)
	, value(value) {
}

RegionCell transformed(const RegionCell& cell, const BlockPosTransform& transform) {
	return { transform(cell.position), cell.value };
}

std::vector<RegionCell> transformed(const std::vector<RegionCell>& regionCells, const BlockPosTransform& transform) {
	std::vector<RegionCell> out;
	out.reserve(regionCells.size());
	for (auto&& cell : regionCells) {
		out.push_back(transformed(cell, transform));
	}
	return out;
}