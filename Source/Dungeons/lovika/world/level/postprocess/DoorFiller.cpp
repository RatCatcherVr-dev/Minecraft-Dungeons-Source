#include "Dungeons.h"
#include "DoorFiller.h"
#include "HeightMapper.h"
#include "lovika/BlockQuery.h"
#include "lovika/BlockPredicates.h"
#include "lovika/Interpolators2.h"
#include "lovika/tile/PlacedTiles.h"
#include "lovika/world/level/ChunkBlockSource.h"
#include "lovika/world/level/LevelGenRandom.h"
#include "world/level/BlockSource.h"

namespace postprocess { namespace door {

static BlockPos getFirstAir(BlockSource& region, BlockPos pos) {
	auto& getter = (ChunkBlockSource&) region;
	const BlockPos max = pos.above(20);
	return blockquery::first(getter, blockpredicates::isBlock(BlockID::AIR), pos, Facing::UP, max).Get(max);
}

struct DoorFrameInfo {
	DoorFrameInfo(BlockSource& source, const DoorDef& door)
		: mSource(source)
		, mDoor(door)
	{
		auto frame = door.doorFrame();
		auto firstTop = getFirstAir(source, frame.first);
		auto secondTop = getFirstAir(source, frame.second);
		for (auto p = frame.first; p.y < firstTop.y; ++p.y) { mFrameBlocks.push_back(source.getBlockAndData(p)); }
		for (auto p = frame.second; p.y < secondTop.y; ++p.y) { mFrameBlocks.push_back(source.getBlockAndData(p)); }
		mHeights = InterpolatorXZ_2<float>(frame.first, (float) firstTop.y, frame.second, (float) secondTop.y);
	}

	std::vector<BlockCuboid> doorColumns() const {
		std::vector<BlockCuboid> columns;
		for (auto basePos : mDoor.positions()) {
			BlockPos topPos(basePos.x, Math::round(mHeights->operator()(basePos)) - 1, basePos.z);
			columns.push_back(BlockCuboid::fromInclusiveCorners(basePos, topPos));
		}
		return columns;
	}

	void fill(const BlockProvider& blockProvider, LevelGenRandom& rnd) const {
		State state{ worldfill::State{{ 0,0,0 }, BlockPos::ZERO, rnd}, mFrameBlocks };
		for (auto column : doorColumns()) {
			state.data.span = HeightMapperSpan(column.minInclusive.y, column.maxExclusive.y, 0);
			for (auto p : column) {
				state.data.pos = p;
				mSource.setBlockAndData(p, blockProvider(state), Block::UPDATE_NONE);
			}
		}
	}
private:
	BlockSource& mSource;
	const DoorDef& mDoor;
	TOptional<InterpolatorXZ_2<float>> mHeights;
	std::vector<FullBlock> mFrameBlocks;
};

void fillDoor(BlockSource& source, const DoorDef& door, const BlockProvider& blockProvider, LevelGenRandom& rnd) {
	DoorFrameInfo(source, door).fill(blockProvider, rnd);
}

std::vector<BlockCuboid> fillUnconnectedDoors(BlockSource& source, const PlacedTiles& doorQuery, const PlacementVector& tilePlacements, const BlockProvider& blockProvider) {
	std::vector<BlockCuboid> filledDoors;
	LevelGenRandom rnd(1);

	for (const auto& tilePlacement : tilePlacements) {
		for (auto&& door : tilePlacement.doors()) {
			if (doorQuery.isSpatiallyConnected(door)) {
				continue;
			}
			if (tilePlacement.isPotentialTeleportDoor(door)) {
				continue;
			}
			if (blockProvider) {
				fillDoor(source, door, blockProvider, rnd);
				const auto frame = door.doorFrame();
				filledDoors.push_back(BlockCuboid::fromInclusiveCorners(frame.first, frame.second));
			}
		}
	}

	return filledDoors;
}
}}
