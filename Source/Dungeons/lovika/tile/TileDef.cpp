#include "Dungeons.h"
#include "TileDef.h"
#include "world/level/block/Block.h"
#include "lovika/BlockCuboid.h"
#include "lovika/QuadrantAngle.h"
#include "lovika/BlockPosTransform.h"
#include "util/Algo.hpp"
#include "util/StringUtils.h"

TileDef::TileDef(
	const std::string& id,
	const std::string& objectGroupId,
	const BlockRegion& blocks,
	const std::vector<DoorDef>& doors,
	const std::vector<lovika::Region>& regions,
	const std::vector<Boundary>& boundaries,
	const std::vector<RegionCell>& regionPlane,
	const std::vector<WalkableCell>& walkableCells,
	TOptional<Height> lowestWalkableBlockY,
	const std::string& tagString
)
	: mId(id)
	, mLowerId(Util::toLower(id))
	, mOriginalLowerId(mLowerId.substr(/*length of tile prefix, e.g. '0001-' */ std::min(std::string::size_type{ 5 }, mLowerId.length())))
	, mLowerObjectGroupId(Util::toLower(objectGroupId))
	, mBlocks(blocks)
	, mDoors(doors)
	, mRegions(regions)
	, mBoundaries(boundaries)
	, mRegionPlane(regionPlane)
	, mWalkableCells(walkableCells)
	, mBaseY(0)
	, mLowestWalkableBlockY(lowestWalkableBlockY)
	, mTags(fromCommaSeparatedString(tagString))
{
	for (auto&& door : doors) {
		algo::add_unique(mRegions, lovika::Region(door.name(), regiontype::Door, door.positions(), door.lowerTagString()));
	}

	for (auto&& walkableCell : walkableCells) {
		mWalkablePlane.set(walkableCell.position.x, walkableCell.position.z, walkableCell.value);
	}
}

const std::string& TileDef::id() const {
	return mId;
}

const std::string& TileDef::lowerId() const {
	return mLowerId;
}

const std::string& TileDef::originalLowerId() const {
	return mOriginalLowerId;
}

const std::string& TileDef::lowerObjectGroupId() const {
	return mLowerObjectGroupId;
}

bool TileDef::isTile() const {
	return !isProp();
}

bool TileDef::isProp() const {
	return mDoors.empty();
}

BlockPos TileDef::size() const {
	return mBlocks.size();
}

void TileDef::setBaseY(int baseY) {
	mBaseY = baseY;
}

int TileDef::baseY() const {
	return mBaseY;
}

const BlockRegion& TileDef::blocks() const {
	return mBlocks;
}

const std::vector<DoorDef>& TileDef::doors() const {
	return mDoors;
}

const std::vector<lovika::Region>& TileDef::regions() const {
	return mRegions;
}

const std::vector<Boundary>& TileDef::boundaries() const {
	return mBoundaries;
}

const std::vector<RegionCell>& TileDef::regionPlane() const {
	return mRegionPlane;
}

const std::vector<WalkableCell>& TileDef::walkableCells() const {
	return mWalkableCells;
}

const TerrainGrid<WalkableHeight>& TileDef::walkablePlane() const {
	return mWalkablePlane;
}

TOptional<Height> TileDef::lowestWalkableBlockY() const {
	return mLowestWalkableBlockY;
}

const Tags& TileDef::tags() const {
	return mTags;
}

void TileDef::setPosHACK(const BlockPos& posHACK) {
	mPosHACK = posHACK;
}

const BlockPos& TileDef::posHACK() const {
	return mPosHACK;
}
