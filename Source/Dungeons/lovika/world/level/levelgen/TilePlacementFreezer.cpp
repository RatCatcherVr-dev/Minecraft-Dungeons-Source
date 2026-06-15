#include "Dungeons.h"
#include "TilePlacementFreezer.h"
#include "TileSet.h"
#include "lovika/Region.h"
#include "lovika/BlockPosTransform.h"
#include "lovika/BlockQuery.h"
#include "lovika/Grid2.h"
#include "lovika/OrientationDataCalculator.h"
#include "lovika/world/level/terrain/TerrainCell.h"
#include "util/Algo.h"
#include "Async/ParallelFor.h"

static BlockCuboid mergedBlockCuboid(BlockCuboid a, BlockCuboid b) {
	if (a.isEmpty()) {
		return b;
	}
	if (b.isEmpty()) {
		return a;
	}
	a.expand(b);
	return a;
}

static void pasteInto(BlockRegion& target, const BlockRegion& src, const Placement& placement) {
	// @note: 2017-04-27: We changed from "soft air" (keep existing) to use "hard air" (replace) when overwriting a non-solid block.
	auto transform = blockpostransform::create(src.size(), BlockRegionTransform::GetRotate(placement.rotation), placement.position);
	auto blockRotationTransform = OrientationDataCalculator(placement.rotation);	
	
	{
		
		//see if we can multithread the processing of y regions to speed things up		
		BlockPos RegionSize = src.size();
		int32 NumThreads = FPlatformProcess::SupportsMultithreading() ? FPlatformMisc::NumberOfCores() : 1;
		int32 NumRowsPerThreadMax = 1;
		int32 NumRowsPerThreadMin = 1;
		int32 LastThreadMax = NumThreads;

		if (RegionSize.y > NumThreads)
		{
			const int32 rowsRemainder = RegionSize.y % NumThreads;

			if (rowsRemainder == 0)
			{
				NumRowsPerThreadMin = NumRowsPerThreadMax = RegionSize.y / NumThreads;
			}
			else
			{
				NumRowsPerThreadMin = RegionSize.y / NumThreads;
				NumRowsPerThreadMax = NumRowsPerThreadMin + 1;
				LastThreadMax = rowsRemainder;
			}
		}
		else
		{
			NumThreads = RegionSize.y;
			LastThreadMax = NumThreads;
		}


		
		ParallelFor(NumThreads, [NumThreads, NumRowsPerThreadMax, NumRowsPerThreadMin, LastThreadMax, &src, &target, &transform, &blockRotationTransform, RegionSize](int32 threadIndex)
		{

			const int LowCount = (threadIndex < LastThreadMax) ? 0 : 1;
			const int startY = (LowCount) ? ((LastThreadMax * NumRowsPerThreadMax) + ((threadIndex - LastThreadMax) * NumRowsPerThreadMin)) : (threadIndex * NumRowsPerThreadMax);
			const int EndY = startY + ((LowCount * NumRowsPerThreadMin) + ((1 - LowCount) * NumRowsPerThreadMax));

			BlockPos regionMin(0, startY, 0), regionMax(RegionSize.x, EndY, RegionSize.z);

			for (auto pos : BlockPosIteration::range(regionMin, regionMax))
			{
				auto targetPos = transform(pos);
				auto block = blockRotationTransform(src.getBlock(pos));

				if (block.id == BlockID::AIR) {
					if (block.data == 0xF || (block.data == 0 && !blockquery::isLogicallySolid(target.getBlock(targetPos).getBlock()))) {
						target.setBlock(targetPos, FullBlock::AIR);
					}
				}
				else {
					target(targetPos, block);
				}
			}
		});
		
	}
}

static void trimBlocks(BlockRegion& region, const BlockCuboid& bounds) {
	for (auto pos : bounds) {
		const auto id = region.getBlockId(pos);
		if (id != BlockID::AIR && !Block::mSolid[id] && !Block::mSolid[region.getBlockId(pos.below())]) {
			region.setBlock(pos, BlockID::AIR);
		}
	}
}

static void trimBlocksAboveProp(BlockRegion& region, BlockPos basePos, const TilePlacement& prop) {
	trimBlocks(region, (flattenToHeight(prop.bounds(), prop.bounds().maxExclusive.y, 2) - basePos));
}

struct Compositor {
	Compositor(int x, int y, int w, int h, const std::vector<RegionCell>& base)
		: mCells(x, y, w, h, { BlockPos::ZERO, 0 })
	{
		for (auto& cell : base) {
			mCells.ref(cell.position) = cell;
		}
	}
	void update(const std::vector<RegionCell>& cells) {
		algo::for_each(cells, [this](const RegionCell& cell) {
			auto& dst = mCells.ref(cell.position);

			if (
				TerrainCell(cell.value).isKillzone() || 
				cell.position.y >= dst.position.y && canOverrideValueWith(dst.value, cell)
			) {
				dst = cell;
			}
		});
	}
	const std::vector<RegionCell>& asVector() {
		return mCells.m;
	}
private:
	static bool canOverrideValueWith(uint8_t old, const RegionCell& cell) {
		return !TerrainCell(old).isBoundary();
	}

	Grid2<RegionCell> mCells;
};

TilePlacementFreezer::TilePlacementFreezer()
	: mIndex(0) {}

PlaceResult TilePlacementFreezer::freeze(PlaceResult placeResult) {

	QUICK_SCOPE_CYCLE_COUNTER(STAT_TilePlacementFreezer_freeze_);

	const TilePlacement& tilePlacement = placeResult.tilePlacement;
	const BlockCuboid mergedBounds = mergedBlockCuboid(tilePlacement.bounds(), calculateBounds(tilePlacement.children()));
	ensure(tilePlacement.bounds().area() == mergedBounds.area() && "Props goes outside the tile edges");

	const BlockPos basePos = mergedBounds.minInclusive;
	Unique<BlockRegion> compositedRegion = std::make_unique<BlockRegion>(mergedBounds.size());
	auto compositedRegionPlane = Compositor(basePos.x, basePos.z, mergedBounds.size().x, mergedBounds.size().z, tilePlacement.regionPlane());

	const auto compositeBlocks = [&region = *compositedRegion, basePos](const TilePlacement& tp) {
		pasteInto(region, tp.tile().blocks(), { tp.bounds().minInclusive - basePos, tp.placement().rotation });
	};

	compositeBlocks(tilePlacement);
	for (auto&& child : tilePlacement.children()) {
		compositeBlocks(child);
		compositedRegionPlane.update(child.regionPlane());
		trimBlocksAboveProp(*compositedRegion, basePos, child);
	}

	const int IndexLength = 4;
	auto index = std::to_string(mIndex++);
	if (index.length() < IndexLength) {
		auto leadingZeros = std::string(IndexLength - index.length(), '0');
		index = leadingZeros + index;
	}
	const auto toLocalTransform = blockpostransform::offset(-basePos);
	auto tile = make_unique<TileDef>(
		index + '-' + tilePlacement.tile().id(),
		tilePlacement.tile().lowerObjectGroupId(),
		*compositedRegion,
		transformed(tilePlacement.doors(), toLocalTransform),
		transformed(tilePlacement.regions(), toLocalTransform),
		transformed(tilePlacement.boundaries(), toLocalTransform),
		transformed(compositedRegionPlane.asVector(), toLocalTransform),
		transformed(tilePlacement.walkableCells(), toLocalTransform),
		transformed(tilePlacement.lowestWalkableBlockY(), toLocalTransform), // @todo: needs composited value?
		toCommaSeparatedString(tilePlacement.tile().tags())
	);
	tile->setBaseY(tilePlacement.tile().baseY());
	tile->setPosHACK(tilePlacement.tile().posHACK());

	TileRef tileRef = *tile;
	const MetaTile metaTile({ tilePlacement.metaIdHACK() }, tileRef);
	// @todo: if tilePlacement.originalPlacement() ->  tilePlacement else calculate combined placement
	TilePlacement frozenPlacement(metaTile, { basePos, QuadrantAngle::D0 }, tilePlacement);

	mFrozen.tiles.feed(std::move(tile));
	mFrozen.placeResults.push_back({ frozenPlacement, placeResult.doorPairs });
	return mFrozen.placeResults.back();
}

FreezeResult& TilePlacementFreezer::result() {
	return mFrozen;
}

const FreezeResult& TilePlacementFreezer::result() const {
	return mFrozen;
}

FreezeResult freezeAll(const std::vector<generator::Tile>& tiles) {
	TilePlacementFreezer freezer;

	for (auto&& generatorTile : tiles) {
		freezer.freeze(generatorTile.placeResult);
	}
	return std::move(freezer.result());
}

FreezeResult freezeAllInPlace(std::vector<generator::Tile>& tiles) {
	TilePlacementFreezer freezer;

	for (auto&& generatorTile : tiles) {
		generatorTile.placeResult = freezer.freeze(generatorTile.placeResult);
	}
	return std::move(freezer.result());
}
