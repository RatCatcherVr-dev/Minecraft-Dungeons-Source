#include "Dungeons.h"
#include "HeightMapper.h"
#include "world/Direction.h"
#include "world/level/BlockSource.h"
#include "lovika/FloodFill.h"
#include "lovika/tile/PlacedTiles.h"
#include "lovika/Interpolators3.h"
#include "lovika/Interpolators2.h"
#include "LogMacros.h"
#include "lovika/BlockQuery.h"
#include "Async/ParallelFor.h"

#define DO_HEIGHT_MAPPER_TIMING 0


void GetParallelDistribution(const int iNumValues, int32& NumThreads_out, int32& NumValsPerThreadMin_out, int32& NumValsPerThreadMax_out,  int32& LastThreadMax_out )
{
	int32 NumThreads = FPlatformProcess::SupportsMultithreading() ? FPlatformMisc::NumberOfCores() : 1;	
	int32 NumValsPerThreadMax = 1;
	int32 NumValsPerThreadMin = 1;
	int32 LastThreadMax = NumThreads;

	if (iNumValues > NumThreads)
	{
		const int32 rowsRemainder = iNumValues % NumThreads;

		if (rowsRemainder == 0)
		{
			NumValsPerThreadMin = NumValsPerThreadMax = iNumValues / NumThreads;
		}
		else
		{
			NumValsPerThreadMin = iNumValues / NumThreads;
			NumValsPerThreadMax = NumValsPerThreadMin + 1;
			LastThreadMax = rowsRemainder;
		}
	}
	else
	{
		NumThreads = iNumValues;
		LastThreadMax = NumThreads;
	}

	NumThreads_out = NumThreads;
	NumValsPerThreadMax_out = NumValsPerThreadMax;
	NumValsPerThreadMin_out = NumValsPerThreadMin;
	LastThreadMax_out = LastThreadMax;
}

void GetParallelStartEnd(const int iThreadIndex, int32 NumValsPerThreadMin, int32 NumValsPerThreadMax, int32 LastThreadMax, int32& StartVal, int32& EndVal)
{
	const int LowCount = (iThreadIndex < LastThreadMax) ? 0 : 1;
	StartVal = (LowCount) ? ((LastThreadMax * NumValsPerThreadMax) + ((iThreadIndex - LastThreadMax) * NumValsPerThreadMin)) : (iThreadIndex * NumValsPerThreadMax);
	EndVal = StartVal + ((LowCount * NumValsPerThreadMin) + ((1 - LowCount) * NumValsPerThreadMax));
}


static inline bool contains(const HeightMapper::Set& positions, const BlockPos& pos) {
	return positions.find(pos) != end(positions);
}

#if DO_HEIGHT_MAPPER_TIMING
static std::vector<double> sHeightMapperTimestamps;

void HeightMapperTimingStart()
{
	sHeightMapperTimestamps.clear();
	sHeightMapperTimestamps.push_back(FPlatformTime::Seconds());
}

static auto HmMeasure = [](const std::string& s) {
	sHeightMapperTimestamps.push_back(FPlatformTime::Seconds());
	double last = sHeightMapperTimestamps.back() - sHeightMapperTimestamps[sHeightMapperTimestamps.size() - 2];
	double total = sHeightMapperTimestamps.back() - sHeightMapperTimestamps.front();

	UE_LOG(LogTemp, Warning, TEXT("HeightMapper: %s: %f (%f)"), *FString(s.c_str()), last, total);
};
#else

static auto HmMeasure = [](const std::string& s) {};
void HeightMapperTimingStart(){}

#endif


HeightMapper::HeightMapper(BlockSource& region, const PlacedTiles& doorQuery, const PlacementVector& placements, int maxDistance)
	: mRegion(region)
	, mDoorQuery(doorQuery)
	, mCells(doorQuery.bounds().maxExclusive.x + maxDistance, doorQuery.bounds().maxExclusive.z + maxDistance)
	, mMaxDistance(maxDistance)
	, mMaxDistanceSquared(maxDistance*maxDistance)
	, mMaxIntermediateDistance(std::abs(maxDistance) + 20)
	, mCurrentRect(growed(calculateBounds(placements), BlockPos(maxDistance, 0, maxDistance)))
{
	run(placements);
}

void HeightMapper::run(const PlacementVector& tiles) {
	QUICK_SCOPE_CYCLE_COUNTER(STAT_postprocess_HeightMapper_run);
	HeightMapperTimingStart();

	auto all = fillData(tiles);
	HmMeasure("fillData");

	auto contour = floodfill::getContour(all);
	HmMeasure("getContour");

	for (const BlockPos& pos : contour) {
		mCells.ref(pos).plane = HalfPlane::Edge;
	}

	calculateNeighbourDistances(contour);
	HmMeasure("calculateNeighbourDistances");

	calculateDistanceGradients(contour);
	HmMeasure("calculateDistanceGradients");

	calculateSlopes();
	HmMeasure("calculateSlopes");

	smooth(15, 0.9f);
	HmMeasure("smooth");

}

HeightMapperSpan HeightMapper::get(int x, int z) const {
	if (mCells.inRange(x, z)) {
		auto& cell = mCells.ref(x, z);
		auto top = cell.height;

		//if (top > 200) { SlopeData sd = mSlope.get(x, z); calculateSlopeHeight(sd); UNUSED(sd); }
		if (cell.plane == HalfPlane::Outside && top > 0) {
			auto min = top;
			const CellData dummy{};
			if (auto h = mCells.refOrDefault(x - 1, z, dummy).height) { min = std::min(h, min); }
			if (auto h = mCells.refOrDefault(x, z - 1, dummy).height) { min = std::min(h, min); }
			if (auto h = mCells.refOrDefault(x + 1, z, dummy).height) { min = std::min(h, min); }
			if (auto h = mCells.refOrDefault(x, z + 1, dummy).height) { min = std::min(h, min); }
			return{ Math::round(min), Math::round(top + 1), sqrt(cell.distanceSquared) };
		}
	}
	return{ 0, 0, 1e6 };
}

BlockCuboid HeightMapper::bounds2D() const {
	const BlockCuboid cellBounds({ mCells.rect().x, 0, mCells.rect().y }, { mCells.rect().w, 1, mCells.rect().h });
	const BlockCuboid rectBounds = flatten(mCurrentRect, 0);
	return cellBounds.intersection(rectBounds);
}

const std::vector<TileArea>& HeightMapper::tileAreas() const {
	return mTileAreas;
}

static BlockPos getTopBlockPos(BlockSource& blocks, BlockPos p) {
	for (; p.y >= 0; --p.y) {
		if (blockquery::isLogicallySolid(blocks.getBlock(p))) {
			break;
		}
	}
	return p;
}

static bool stripContour(HeightMapper::Set& inside, int lowerHeightLimit) {
	auto oldSize = inside.size();
	auto contour = floodfill::getContour(inside);
	for (auto p : contour) {
		if (p.y < lowerHeightLimit) {
			inside.erase(p);
		}
	}
	return oldSize != inside.size();
}

static void removeBlocksOutsideWalls(HeightMapper::Set& inside, int lowerHeightLimit) {
	int maxIterations = 5;
	while (--maxIterations >= 0 && stripContour(inside, lowerHeightLimit)) {}
}

HeightMapper::Set HeightMapper::fillData(const PlacementVector& tiles) {
	Set all;
	Set tileAll;

	for (auto& tp : tiles) {
		tileAll.clear();

		const auto bounds = tp.bounds();
		auto min = bounds.minInclusive;
		auto max = bounds.maxExclusive;
		min.y = max.y - 1;
		for (auto p : BlockPosIteration::range(min, max)) {
			auto topBlockPos = getTopBlockPos(mRegion, p);
			if (topBlockPos.y > 0) {
				tileAll.emplace(topBlockPos);
			}
		}
		for (auto&& door : tp.doors()) {
			if (!mDoorQuery.isSpatiallyConnected(door)) {
				continue;
			}
			auto frame = door.doorFrame();
			InterpolatorXZ_2<float> doorHeights(frame.first,
				(float) getTopBlockPos(mRegion, frame.first).y, frame.second,
				(float) getTopBlockPos(mRegion, frame.second).y
			);

			for (auto&& doorPos : door.positions()) {
				tileAll.erase(doorPos);
				tileAll.emplace(doorPos.above(Math::round(doorHeights(doorPos)) - doorPos.y));
			}
		}
		removeBlocksOutsideWalls(tileAll, tp.placement().position.y + tp.tile().baseY());
		mTileAreas.push_back(calculateTileArea(tp.bounds(), tileAll));
		all.insert(begin(tileAll), end(tileAll));
	}

	for (auto pos : all) {
		auto& cell = mCells.ref(pos.x, pos.z);
		cell.height = (float) pos.y;
		cell.plane = HalfPlane::Inside;
	}
	return all;
}

namespace {
struct Pt {
	Pt() {}
	Pt(Vec3 p, float h) : p(p), h(h), d(std::abs(p.x + p.z)) {}
	Vec3 p;
	float h, d;
};
}

struct SmoothCache {
	struct Pos {
		int index;
		unsigned int neighbourMask;
		float inverseCount;
	};
	SmoothCache(int width, int height) {
		has.resize(width * height, false);
		positions.reserve(10000);
	}

	std::vector<Pos> positions;
	std::basic_string<bool> has;
};

void HeightMapper::updateSmoothCache(SmoothCache& cache)
{

	QUICK_SCOPE_CYCLE_COUNTER(STAT_postprocess_HeightMapper_updateSmoothCache);
	const IntRect rect = mCells.rect(); // @todo: use local bounds
	
	int32 NumThreads = 1;
	int32 NumRowsPerThreadMax = 1;
	int32 NumRowsPerThreadMin = 1;
	int32 LastThreadMax = NumThreads;

	GetParallelDistribution(rect.h, NumThreads, NumRowsPerThreadMin, NumRowsPerThreadMax, LastThreadMax);

	{
		TQueue< SmoothCache::Pos, EQueueMode::Mpsc > PosOutput;

		ParallelFor(NumThreads, [&, NumThreads, NumRowsPerThreadMin, NumRowsPerThreadMax, LastThreadMax](int32 threadIndex)
		{
			int32 StartVal = 0, EndVal = 0;
			GetParallelStartEnd(threadIndex, NumRowsPerThreadMin, NumRowsPerThreadMax, LastThreadMax, StartVal, EndVal);

			int index = rect.w * StartVal; //D11.PS ensure this is an int or it overruns.

			const unsigned short startZ = StartVal;
			const unsigned short EndZ = EndVal;
			const unsigned short lastX = rect.w - 1;
			const unsigned short lastZ = rect.h - 1;

			for (unsigned short z = startZ; z < EndZ; ++z)
			{
				for (unsigned short x = 0; x < rect.w; ++x, ++index)
				{
					if (cache.has[index]) {
						continue;
					}
					
					
					unsigned int neighbourMask = 0;
					float c = 0;

					if(x > 0)
					{
						auto& nb = mCells.m[index-1];
						if (nb.height && nb.plane != HalfPlane::Inside) {
							c += 1;
							neighbourMask |= 1 << Direction::WEST;
						}
					}
					if (x < lastX)
					{
						auto& nb = mCells.m[index + 1];
						if (nb.height && nb.plane != HalfPlane::Inside) {
							c += 1;
							neighbourMask |= 1 << Direction::EAST;
						}
					}

					if (z > 0)
					{
						auto& nb = mCells.m[index - rect.w];
						if (nb.height && nb.plane != HalfPlane::Inside) {
							c += 1;
							neighbourMask |= 1 << Direction::NORTH;
						}
					}
					if (z < lastZ)
					{
						auto& nb = mCells.m[index + rect.w];
						if (nb.height && nb.plane != HalfPlane::Inside) {
							c += 1;
							neighbourMask |= 1 << Direction::SOUTH;
						}
					}

					if (neighbourMask > 0) {
						const SmoothCache::Pos pos{ index, neighbourMask, 1.0f / c };
						PosOutput.Enqueue(pos);
					}
				}
			}

		});

		//deque values from thread
		SmoothCache::Pos pos;
		while (PosOutput.Dequeue(pos))
		{
			cache.positions.push_back(pos);
			cache.has[pos.index] = true;
		}

	}


}

void HeightMapper::smooth(int iterations, float smoothFraction) {
	QUICK_SCOPE_CYCLE_COUNTER(STAT_postprocess_HeightMapper_smooth);
	if (iterations <= 0) {
		return;
	}
	
	SmoothCache cache(mCells.width(), mCells.height()); // @todo: use local bounds

	//preprocess, mark off all non outside cells in cache to prevent checking them all the time	
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_postprocess_HeightMapper_smooth_preprocess_multi);
		const int iCells = mCells.m.size();
		int32 NumThreads = 1;
		int32 NumValsPerThreadMax = 1;
		int32 NumValsPerThreadMin = 1;
		int32 LastThreadMax = 1;

		GetParallelDistribution(iCells, NumThreads, NumValsPerThreadMin, NumValsPerThreadMax, LastThreadMax);

		ParallelFor(NumThreads, [&, NumThreads, NumValsPerThreadMin, NumValsPerThreadMax, LastThreadMax](int32 threadIndex)
		{
			int32 StartVal = 0, EndVal = 0;
			GetParallelStartEnd(threadIndex, NumValsPerThreadMin, NumValsPerThreadMax, LastThreadMax, StartVal, EndVal);

			for (int i = StartVal; i < EndVal; ++i)
			{
				if (mCells.m[i].plane != HalfPlane::Outside)
				{
					cache.has[i] = true;
				}
			}
		});
	}

	Grid TempCells = mCells;
	for (int i = 0; i < iterations; ++i) {
		QUICK_SCOPE_CYCLE_COUNTER(STAT_postprocess_HeightMapper_iterate);
		updateSmoothCache(cache);
		mCells.m.swap(TempCells.m);
		smoothOnce(mCells, TempCells,smoothFraction, cache);
	}
}

void HeightMapper::smoothOnce(Grid& result, Grid& source, float smoothFraction, const SmoothCache& cache) {	
	QUICK_SCOPE_CYCLE_COUNTER(STAT_postprocess_HeightMapper_smoothOnce);
	const IntRect rect = source.rect();
	const int width = rect.w;	
	
	int32 NumThreads = 1;
	int32 NumValsPerThreadMax = 1;
	int32 NumValsPerThreadMin = 1;
	int32 LastThreadMax = 1;

	GetParallelDistribution(cache.positions.size(), NumThreads, NumValsPerThreadMin, NumValsPerThreadMax, LastThreadMax);

	ParallelFor(NumThreads, [&, NumThreads, NumValsPerThreadMin, NumValsPerThreadMax, LastThreadMax](int32 threadIndex)
	{
		int32 StartVal = 0, EndVal = 0;
		GetParallelStartEnd(threadIndex, NumValsPerThreadMin, NumValsPerThreadMax, LastThreadMax, StartVal, EndVal);

		for (int i = StartVal; i < EndVal; ++i)
		{
			const auto& pos = cache.positions[i];

			const auto& cell = source.m[pos.index];

			float th = 0;

			if (pos.neighbourMask & (1 << Direction::WEST)) th += source.m[pos.index - 1].height;
			if (pos.neighbourMask & (1 << Direction::EAST)) th += source.m[pos.index + 1].height;
			if (pos.neighbourMask & (1 << Direction::NORTH)) th += source.m[pos.index - width].height;
			if (pos.neighbourMask & (1 << Direction::SOUTH)) th += source.m[pos.index + width].height;

			const float oldHeight = cell.height;
			const float newHeight = th * pos.inverseCount;

			result.m[pos.index].height = oldHeight > 0 ? newHeight * smoothFraction + oldHeight * (1 - smoothFraction) : newHeight;
		}
	});
	

}

void HeightMapper::calculateNeighbourDistances(const std::vector<BlockPos>& contour) {
	QUICK_SCOPE_CYCLE_COUNTER(STAT_postprocess_HeightMapper_calculateNeighbourDistances);
	const IntRect rect = mCells.rect(); // @todo: use local bounds?
	const int x1 = rect.x1();
	const int y1 = rect.y1();
	
	for (const BlockPos& pos : contour) {
		unsigned char height = pos.y;

		{
			int dist = 0;
			BlockPos p(pos);
			while (dist <= mMaxIntermediateDistance && --p.x >= rect.x && mCells.ref(p).height == 0) {
				auto& slope = mCells.ref(p).slope;
				slope.dirDist[Direction::WEST] = ++dist;
				slope.dirHeight[Direction::WEST] = height;
			}
		}
		{
			int dist = 0;
			BlockPos p(pos);
			while (dist <= mMaxIntermediateDistance && --p.z >= rect.y && mCells.get(p).height == 0) {
				auto& slope = mCells.ref(p).slope;
				slope.dirDist[Direction::NORTH] = ++dist;
				slope.dirHeight[Direction::NORTH] = height;
			}
		}
		{
			int dist = 0;
			BlockPos p(pos);
			while (dist <= mMaxIntermediateDistance && ++p.x < x1 && mCells.get(p).height == 0) {
				auto& slope = mCells.ref(p).slope;
				slope.dirDist[Direction::EAST] = ++dist;
				slope.dirHeight[Direction::EAST] = height;
			}
		}
		{
			int dist = 0;
			BlockPos p(pos);
			while (dist <= mMaxIntermediateDistance && ++p.z < y1 && mCells.get(p).height == 0) {
				auto& slope = mCells.ref(p).slope;
				slope.dirDist[Direction::SOUTH] = ++dist;
				slope.dirHeight[Direction::SOUTH] = height;
			}
		}
	}
}

namespace heightmapperinternal {
	struct Offset { int x, y; };

	static std::array<Offset, 4> offsets{ {
		{-1, 0 },
		{ 1, 0 },
		{ 0,-1 },
		{ 0, 1 },
	} };
}

void HeightMapper::calculateDistanceGradients(const std::vector<BlockPos>& zeroDistanceBlocks) {
	QUICK_SCOPE_CYCLE_COUNTER(STAT_postprocess_HeightMapper_calculateDistanceGradients);

	const int iDimensions = (mMaxDistance * 2) + 1;
	
	std::unique_ptr<char[]> Validitymap(new char[iDimensions*iDimensions]);

	for (auto origin : zeroDistanceBlocks) {
		QUICK_SCOPE_CYCLE_COUNTER(STAT_postprocess_HeightMapper_calculateDistanceGradients_squareflood);
		_squareflood(origin, Validitymap.get());
		
	}
	
	
}

void HeightMapper::_squareflood(BlockPos& origin, char *ValidityMapCache)
{
	if (mMaxDistance == 0)
	{
		return;
	}

	const BlockPos StartPos(origin.x - mMaxDistance, origin.y, origin.z - mMaxDistance);
	const BlockPos EndPos(origin.x + mMaxDistance, origin.y, origin.z + mMaxDistance);

	const int iDimensions = (mMaxDistance * 2) + 1;

	//lets create the cache locally if its not around
	char* Validitymap = nullptr;
	if (!ValidityMapCache)
	{
		Validitymap = new char[iDimensions*iDimensions];
		ValidityMapCache = Validitymap;
	}

	//clear out Validity Cache
	FMemory::Memzero(ValidityMapCache, iDimensions*iDimensions);

	const IntRect CellsRect = mCells.rect();

	auto CellCheckFunc = [&](const heightmapperinternal::Offset& pt)->bool
	{
		ValidityMapCache[(pt.y*iDimensions) + (pt.x)] = 1; //done this one

		const BlockPos pos(StartPos.x + pt.x, StartPos.y, StartPos.z + pt.y);
		const int XRect(pos.x - CellsRect.x);
		const int ZRect(pos.z - CellsRect.y);

		if (XRect < 0 || XRect >= CellsRect.w)
		{
			//outside rect width
			return false;
		}
		if (ZRect < 0 || ZRect >= CellsRect.h)
		{
			//outside rect height
			return false;
		}

		const float squaredDistance = origin.distSqr(pos);
		if (squaredDistance >= mMaxDistanceSquared)
		{
			return false;
		}


		auto& cell = mCells.ref(pos);
		if (cell.plane != HeightMapper::HalfPlane::Outside)
		{
			return false;
		}

		if (cell.distanceSquared <= squaredDistance)
		{
			return false;
		}

		cell.distanceSquared = squaredDistance;
		return true;
	};

	//we should now have a bitmap of valid cells, lets scanline fill from origin
	std::stack<heightmapperinternal::Offset> points;
	
	{
		//find valid next to origin to fill from
		for (auto ofs : heightmapperinternal::offsets)
		{
			if (CellCheckFunc({ mMaxDistance + ofs.x , mMaxDistance + ofs.y }))
			{
				points.push({ mMaxDistance + ofs.x , mMaxDistance + ofs.y }); // add new start point
				break;
			}
		}
	}


	while (!points.empty())
	{
		heightmapperinternal::Offset pt = points.top();
		points.pop();
		
		heightmapperinternal::Offset left_pt = { pt.x,pt.y };
		heightmapperinternal::Offset right_pt = { pt.x,pt.y };

		//push left
		while (left_pt.x > 0 && !ValidityMapCache[(left_pt.y*iDimensions) + (left_pt.x - 1)])
		{
			if (!CellCheckFunc({ left_pt.x-1,left_pt.y }))
			{
				break;
			}

			--left_pt.x;
		}

		//push right
		while (right_pt.x < (iDimensions - 1) && !ValidityMapCache[(right_pt.y*iDimensions) + (right_pt.x + 1)])
		{		
			if (!CellCheckFunc({ right_pt.x + 1,right_pt.y }))
			{
				break;
			}

			++right_pt.x;
		}


		heightmapperinternal::Offset scan_pt = { left_pt.x, left_pt.y };

		bool check_above = true;
		bool check_below = true;

		for (; scan_pt.x <= right_pt.x; ++scan_pt.x)
		{
			if (scan_pt.y > 0)
			{
				heightmapperinternal::Offset above_pt = { scan_pt.x, scan_pt.y - 1 };

				if (check_above)
				{
					if (!ValidityMapCache[(above_pt.y*iDimensions) + above_pt.x])
					{
						if (CellCheckFunc(above_pt))
						{
							points.push(above_pt);
							check_above = false;
						}
					}
				}
				else
				{
					check_above = (!ValidityMapCache[(above_pt.y*iDimensions) + above_pt.x] && !CellCheckFunc(above_pt));
				}
			}

			if (scan_pt.y < (iDimensions - 1))
			{
				heightmapperinternal::Offset below_pt = { scan_pt.x, scan_pt.y + 1 };

				if (check_below)
				{
					if (!ValidityMapCache[(below_pt.y*iDimensions) + below_pt.x])
					{
						if (CellCheckFunc(below_pt))
						{
							points.push(below_pt);
							check_below = false;
						}
					}
				}
				else // !check_below
				{
					check_below = (!ValidityMapCache[(below_pt.y*iDimensions) + below_pt.x] && !CellCheckFunc(below_pt));
				}
			}
		}
	}

	//remove validity cache if we set it locally
	if (Validitymap)
		delete[] Validitymap;
	

}

void HeightMapper::_flood(BlockPos origin, BlockPos pos) {

	const float squaredDistance = origin.distSqr(pos);

	if (squaredDistance >= mMaxDistanceSquared || !mCells.inRange(pos.x, pos.z)) { // @todo: use local bounds
		return;
	}
	auto& cell = mCells.ref(pos);
	if (cell.plane != HeightMapper::HalfPlane::Outside) {
		return;
	}

	
	if (cell.distanceSquared <= squaredDistance) {
		return;
	}
	cell.distanceSquared = squaredDistance;
	for (auto ofs : heightmapperinternal::offsets) {
		_flood(origin, pos.offset(ofs.x, 0, ofs.y));
	}
}



void HeightMapper::calculateSlopes() {
	QUICK_SCOPE_CYCLE_COUNTER(STAT_postprocess_HeightMapper_calculateSlopes);

		
	const int iNumCells = mCells.m.size();

	if (iNumCells == 0)
	{
		return;
	}

	int32 NumThreads = 1;
	int32 NumValsPerThreadMax = 1;
	int32 NumValsPerThreadMin = 1;
	int32 LastThreadMax = 1;

	GetParallelDistribution(iNumCells, NumThreads, NumValsPerThreadMin, NumValsPerThreadMax, LastThreadMax);

	ParallelFor(NumThreads, [&, NumThreads, NumValsPerThreadMin, NumValsPerThreadMax, LastThreadMax](int32 threadIndex)
	{
		int32 StartVal = 0, EndVal = 0;
		GetParallelStartEnd(threadIndex, NumValsPerThreadMin, NumValsPerThreadMax, LastThreadMax, StartVal, EndVal);

		for (int32 i = StartVal; i < EndVal; ++i)
		{
			auto& cell = mCells.m[i];
			if (cell.plane == HalfPlane::Outside) {
				cell.height = calculateSlopeHeight(cell.slope);
			}
		}
	});
	
}

float HeightMapper::calculateSlopeHeight(const SlopeData& s) const {
	int count = 0;

	std::array<Pt, 4> pts;
	if (auto h = s.dirHeight[Direction::WEST]) {
		pts[count++] = Pt(BlockPos(-s.dirDist[Direction::WEST], 0, 0), (float) h);
	}
	if (auto h = s.dirHeight[Direction::NORTH]) {
		pts[count++] = Pt(BlockPos(0, 0, -s.dirDist[Direction::NORTH]), (float) h);
	}
	if (auto h = s.dirHeight[Direction::EAST]) {
		pts[count++] = Pt(BlockPos(s.dirDist[Direction::EAST], 0, 0), (float) h);
	}
	if (auto h = s.dirHeight[Direction::SOUTH]) {
		pts[count++] = Pt(BlockPos(0, 0, s.dirDist[Direction::SOUTH]), (float) h);
	}
	if (count == 0) { return 0; }
	if (count == 1) { return pts[0].h; }
	if (count == 2) {
		float t = pts[0].d / (pts[0].d + pts[1].d);
		return pts[1].h * t + pts[0].h * (1 - t);
		//return InterpolatorXZ_2<float>(pts[0].p, pts[0].h, pts[1].p, pts[1].h)(Vec3::ZERO);
	}
	if (count == 3) {
		return InterpolatorXZ_3<float>(pts[0].p, pts[0].h, pts[1].p, pts[1].h, pts[2].p, pts[2].h)(Vec3::ZERO);
	}
	InterpolatorXZ_3<float> a(pts[0].p, pts[0].h, pts[1].p, pts[1].h, pts[2].p, pts[2].h),
		b(pts[0].p, pts[0].h, pts[2].p, pts[2].h, pts[3].p, pts[3].h);
	return (a(Vec3::ZERO) + b(Vec3::ZERO)) * 0.5f;
}

TileArea HeightMapper::calculateTileArea(const BlockCuboid& volume, const Set& built) const {
	auto wallCount = floodfill::getContour(built).size();
	auto floorCount = built.size() - wallCount;
	auto dimensions = volume.size();
	Grid2<Height> heights(volume.minInclusive.x, volume.minInclusive.z, dimensions.x, dimensions.z);
	for (auto&& pos : built) {
		heights.set(pos.x, pos.z, pos.y + 1);
	}
	return { volume.area(), static_cast<uint32>(floorCount), static_cast<uint32>(built.size()), heights };
}

