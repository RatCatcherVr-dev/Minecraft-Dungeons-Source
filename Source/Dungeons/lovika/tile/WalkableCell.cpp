#include "Dungeons.h"
#include "WalkableCell.h"

WalkableCell::WalkableCell(const BlockPos& position, WalkableHeight value)
	: position(position)
	, value(value) {
}

WalkableCell transformed(const WalkableCell& cell, const BlockPosTransform& transform) {
	const BlockPos transformedPos = transform({ cell.position.x, 0, cell.position.z });
	return { transformedPos, cell.value.withOffset(transformedPos.y) };
}

std::vector<WalkableCell> transformed(const std::vector<WalkableCell>& walkableCells, const BlockPosTransform& transform) {
	std::vector<WalkableCell> out;
	out.reserve(walkableCells.size());
	for (auto&& cell : walkableCells) {
		out.push_back(transformed(cell, transform));
	}
	return out;
}

TOptional<Height> transformed(TOptional<Height> height, const BlockPosTransform& transform) {
	return height ? transform({ 0, height.GetValue(), 0 }).y : TOptional<Height>{};
}
