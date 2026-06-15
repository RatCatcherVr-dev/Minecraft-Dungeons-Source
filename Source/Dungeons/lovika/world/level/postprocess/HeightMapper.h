#pragma once

#include "lovika/Grid2.h"
#include "lovika/BlockCuboid.h"
#include "lovika/tile/TileArea.h"
#include "world/level/BlockPos.h"

class BlockSource;
class PlacedTiles;
class TilePlacement;
using PlacementVector = std::vector<TilePlacement>;
struct SmoothCache;

struct HeightMapperSpan {
	HeightMapperSpan(int min, int max, float distance) : min(min), max(max), distance(distance) {}
	int size() const { return max - min; }
	bool isEmpty() const { return min == max; }
	int min;
	int max;
	float distance;
};

class HeightMapper {
public:
	using Set = std::unordered_set<BlockPos, BlockXzComparator, BlockXzComparator>;

	HeightMapper(BlockSource&, const PlacedTiles& doorQuery, const PlacementVector&, int maxDistance);

	HeightMapperSpan get(int x, int z) const;
	BlockCuboid bounds2D() const;

	const std::vector<TileArea>& tileAreas() const;
private:
	struct SlopeData {
		short dirDist[4] = { 0 };
		unsigned char dirHeight[4] = { 0 };
	};
	enum class HalfPlane {
		Outside,
		Edge,
		Inside
	};
	struct CellData {
		float height = 0;
		float distanceSquared = 1e6;
		HalfPlane plane = HalfPlane::Outside;

		SlopeData slope;
	};
	using Grid = Grid2<CellData>;

	void run(const PlacementVector&);
	Set fillData(const PlacementVector&);
	void calculateNeighbourDistances(const std::vector<BlockPos>&);
	void calculateDistanceGradients(const std::vector<BlockPos>& zeroDistanceBlocks);
	void calculateSlopes();
	float calculateSlopeHeight(const SlopeData&) const;
	TileArea calculateTileArea(const BlockCuboid&, const Set&) const;
	void smooth(int iterations, float smoothFraction);
	void smoothOnce(Grid& result, Grid& source, float smoothFraction, const SmoothCache&);
	void updateSmoothCache(SmoothCache&);
	void _flood(BlockPos origin, BlockPos pos);
	void _squareflood(BlockPos& origin, char *ValidityMapCache = nullptr);

	BlockSource& mRegion;
	const PlacedTiles& mDoorQuery;
	Grid mCells;
	BlockCuboid mCurrentRect;
	std::vector<TileArea> mTileAreas;
	int mMaxDistance;
	int mMaxDistanceSquared;
	int mMaxIntermediateDistance;
};
