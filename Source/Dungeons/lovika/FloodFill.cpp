#include "Dungeons.h"
#include "FloodFill.h"
#include "BlockCuboid.h"

/************************************************************************/
/*  Flood Fill                                                          */
/************************************************************************/
static inline bool contains(const std::unordered_set<BlockPos>& positions, const BlockPos& pos) {
	return positions.find(pos) != end(positions);
}

int floodfill::forEachPos(const BlockPos& start, DirectionMask directionMask, PosPredicate matches, PosCallback posCallback) {
	std::unordered_set<BlockPos> seen;
	std::vector<BlockPos> pending;
	int filledCount = 0;
	pending.push_back(start);
	for (unsigned int i = 0; i < pending.size(); ++i) {
		const BlockPos pos = pending[i];
		if (contains(seen, pos)) {
			continue;
		}

		BlockPos lo = pos;
		if (directionMask & DirectionMask::X_NEG) {
			do { --lo.x; } while (!contains(seen, lo) && matches(lo));
			++lo.x;
		}

		BlockPos hi = pos;
		if (directionMask & DirectionMask::X_POS) {
			do { ++hi.x; } while (!contains(seen, hi) && matches(hi));
			--hi.x;
		}

		for (auto p = lo; p.x <= hi.x; ++p.x) {
			seen.insert(p);
			posCallback(p);

			if (directionMask & DirectionMask::Y_NEG) {
				auto below = p.below();
				if (!contains(seen, below) && matches(below)) {
					pending.push_back(below);
				}
			}
			if (directionMask & DirectionMask::Y_POS) {
				auto above = p.above();
				if (!contains(seen, above) && matches(above)) {
					pending.push_back(above);
				}
			}
			if (directionMask & DirectionMask::Z_NEG) {
				auto north = p.north();
				if (!contains(seen, north) && matches(north)) {
					pending.push_back(north);
				}
			}
			if (directionMask & DirectionMask::Z_POS) {
				auto south = p.south();
				if (!contains(seen, south) && matches(south)) {
					pending.push_back(south);
				}
			}
		}
	}
	return filledCount;
}

std::vector<BlockPos> floodfill::getAllPos(const BlockPos& start, DirectionMask directionMask, PosPredicate matches) {
	std::vector<BlockPos> positions;
	forEachPos(start, directionMask, matches, [&](BlockPos pos) {
		positions.push_back(pos);
	});
	return positions;
}

static int _fillSameType(const BlockGetter& blocks, const BlockPos& start, DirectionMask directionMask, FacingID facing, PosCallback posCallback) {
	const BlockCuboid bounds = cuboidFromCenter(start, 50);
	const auto targetBlockId = blocks(start).id;

	PosPredicate predicate;
	if (targetBlockId != BlockID::AIR && facing != Facing::NOT_DEFINED) {
		predicate = [&](auto pos) {
			return targetBlockId == blocks(pos).id && blocks(pos.neighbor(facing)).id == BlockID::AIR;
		};
	} else {
		predicate = [&](auto pos) {
			return targetBlockId == blocks(pos).id;
		};
	}

	bool valid = true;
	auto positions = floodfill::getAllPos(start, directionMask, [&](auto pos) {
		if (!valid) {
			return false;
		}
		valid &= bounds.contains(pos);
		return valid && predicate(pos);
	});
	if (valid) {
		std::for_each(begin(positions), end(positions), posCallback);
		return positions.size();
	}
	return 0;
}

int floodfill::fillSameType(const BlockGetter& blocks, const BlockPos& start, FacingID facing, PosCallback posCallback) {
	return _fillSameType(blocks, start, DirectionMask::PLANE_FACING[facing], facing, posCallback);
}

int floodfill::fillSameType(const BlockGetter& blocks, const BlockPos& start, DirectionMask directionMask, PosCallback posCallback) {
	return _fillSameType(blocks, start, directionMask, Facing::NOT_DEFINED, posCallback);
}

std::vector<BlockPos> floodfill::getContour(const BlockGetter& region, const BlockPos& start) {
	std::unordered_set<BlockPos> all;
	floodfill::fillSameType(region, start, DirectionMask::XZ, [&](BlockPos pos) {
		all.insert(pos);
	});
	return getContour(all);
}
