#pragma once
#include "world/level/BlockPos.h"
#include "DirectionMask.h"

namespace floodfill {
	int forEachPos(const BlockPos&, DirectionMask, PosPredicate, PosCallback);
	std::vector<BlockPos> getAllPos(const BlockPos&, DirectionMask, PosPredicate);

	int fillSameType(const BlockGetter&, const BlockPos&, DirectionMask, PosCallback);
	/// Same as above, but also checks the block at direction FacingID (e.g. check the block above)
	int fillSameType(const BlockGetter&, const BlockPos&, FacingID, PosCallback);

	template <typename... Args>
	std::vector<BlockPos> getContour(const std::unordered_set<BlockPos, Args...>&);
	std::vector<BlockPos> getContour(const BlockGetter&, const BlockPos&);
}

template <typename... Args>
std::vector<BlockPos> floodfill::getContour(const std::unordered_set<BlockPos, Args...>& all) {
	std::vector<BlockPos> contour;
	const auto end = std::end(all);
	for (auto p : all) {
		if (all.find(p.west()) == end || all.find(p.east()) == end || all.find(p.north()) == end || all.find(p.south()) == end) {
			contour.push_back(p);
		}
	}
	return contour;
}
