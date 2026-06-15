#pragma once
#include "CommonTypes.h"
#include "world/level/BlockPos.h"
#include "world/Facing.h"

namespace blockquery {
	using Result = TOptional<BlockPos>;

	template <class _BlockGetter, class _Predicate>
	Result first(_BlockGetter&&, _Predicate&&, BlockPos startPos, FacingID, BlockPos invalidPos);
	template <class _BlockGetter, class _Predicate>
	Result last(_BlockGetter&&, _Predicate&&, BlockPos startPos, FacingID, BlockPos invalidPos);

	bool isLogicallySolid(const Block&);
}

//
// Impl
//
template <class _BlockGetter, class _Predicate>
blockquery::Result blockquery::first(_BlockGetter&& getter, _Predicate&& predicate, BlockPos start, FacingID facing, BlockPos invalidPos) {
	const BlockPos offset = Facing::DIRECTION[facing];

	BlockPos pos = start;
	while (pos != invalidPos) {
		auto block = getter(pos);
		if (predicate(pos, block)) {
			return pos;
		}
		pos += offset;
	}
	return {};
}

template <class _BlockGetter, class _Predicate>
blockquery::Result blockquery::last(_BlockGetter&& getter, _Predicate&& predicate, BlockPos start, FacingID facing, BlockPos invalidPos) {
	const BlockPos offset = Facing::DIRECTION[facing];

	BlockPos pos = start;
	while (pos != invalidPos) {
		auto block = getter(pos);
		if (!predicate(pos, block)) {
			return (pos != start) ? pos.neighbor(Facing::OPPOSITE_FACING[facing]) : blockquery::Result();
		}
		pos += offset;
	}
	return pos;
}
