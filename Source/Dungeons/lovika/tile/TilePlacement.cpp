#include "Dungeons.h"

#include "TileDef.h"
#include "TilePlacement.h"
#include "lovika/BlockRegionTransform.h"
#include "lovika/BlockPosTransform.h"
#include "util/algo.h"
#include <numeric>

TilePlacement::TilePlacement(const std::string& metaTileId, TileRef tile, Placement placement, TOptional<TilePlacement> originalPlacement /*= {}*/)
	: mMetaId(metaTileId)
	, mTile(&tile)
	, mPlacement(placement)
	, mIdHACK(tile.id())
{
	if (originalPlacement) {
		mOriginalPlacement.push_back(originalPlacement.GetValue());
	}
	_buildTransform();
}

TilePlacement::TilePlacement(const MetaTile& tile, Placement placement, TOptional<TilePlacement> originalPlacement /*= {} */)
	: TilePlacement(tile.id, tile.tile(), placement, originalPlacement) {}

TilePlacement::~TilePlacement() {
}

std::vector<DoorDef> TilePlacement::doors() const {
	std::vector<DoorDef> doors;
	for (auto& door : tile().doors()) {
		doors.push_back(transformedDoor(*this, door));
	}
	return doors;
}

std::vector<lovika::Region> TilePlacement::regions() const {
	std::vector<lovika::Region> regions;
	_regions(regions);
	return regions;
}

std::vector<lovika::Region> TilePlacement::filterRegions(const RegionPredicate& predicate) const {
	std::vector<lovika::Region> regions;
	_filterRegions(predicate, regions);
	return regions;
}

bool TilePlacement::hasRegion(const RegionPredicate& pred) const {
	return algo::any_of(tile().regions(), pred) || algo::any_of(mChildren, [&](const TilePlacement& it) { return it.hasRegion(pred);});
}


TOptional<Height> TilePlacement::lowestWalkableBlockY() const {
	return transformed(mTile->lowestWalkableBlockY(), mBlockPosTransform);
}

bool TilePlacement::isPotentialTeleportDoor(const DoorDef& door) const {
	return mInnerBounds.containsXZ(door.positions());
}

void TilePlacement::_regions(std::vector<lovika::Region>& out) const {
	for (const lovika::Region& region : tile().regions()) {
		out.push_back(transformed(region, mBlockPosTransform));
	}
	for (const TilePlacement& child : mChildren) {
		child._regions(out);
	}
}

void TilePlacement::_filterRegions(const RegionPredicate& predicate, std::vector<lovika::Region>& out) const {
	for (const lovika::Region& region : tile().regions()) {
		if (predicate(region)) {
			out.push_back(transformed(region, mBlockPosTransform));
		}
	}
	for (const TilePlacement& child : mChildren) {
		child._filterRegions(predicate, out);
	}
}

std::vector<Boundary> TilePlacement::boundaries() const {
	return transformed(tile().boundaries(), mBlockPosTransform);
}

std::vector<RegionCell> TilePlacement::regionPlane() const {
	return transformed(tile().regionPlane(), mBlockPosTransform);
}

std::vector<WalkableCell> TilePlacement::walkableCells() const {
	return transformed(tile().walkableCells(), mBlockPosTransform);
}

TileRef TilePlacement::tile() const {
	return *mTile;
}

const std::string& TilePlacement::tileIdHACK() const {
	return mIdHACK;
}

const std::string& TilePlacement::metaIdHACK() const {
	return mMetaId;
}

Placement TilePlacement::placement() const {
	return mPlacement;
}

TOptional<TilePlacement> TilePlacement::originalPlacement() const {
	return mOriginalPlacement.empty() ? TOptional<TilePlacement>{} : mOriginalPlacement[0];
}

const BlockCuboid& TilePlacement::bounds() const {
	return mBounds;
}

void TilePlacement::translate(BlockPos offset) {
	mPlacement.position += offset;
	_buildTransform();

	for (auto& child : mChildren) {
		child.translate(offset);
	}
}

const BlockPosTransform& TilePlacement::blockPosTransform() const {
	return mBlockPosTransform;
}

PlacementVector& TilePlacement::children() {
	return mChildren;
}

// Helpers for platform independent hashing
namespace hashcodes {

template <typename T>
std::size_t primitive(T t) {
	return static_cast<std::size_t>(t);
}

template <typename Collection>
std::size_t collectionPrimitiveElements(const Collection& collection) {
	return std::accumulate(collection.begin(), collection.end(), std::size_t{ 0 }, [](std::size_t hash, const auto& it) { return hash * 31 + primitive(it); } );
}

template <typename Collection>
std::size_t collectionHashCodeElements(const Collection& collection) {
	return std::accumulate(collection.begin(), collection.end(), std::size_t{ 0 }, [](std::size_t hash, const auto& it) { return hash * 31 + it.hashCode(); });
}

inline void combine(std::size_t& combined, std::size_t add) {
	combined = 1009 * combined + add;
}

}

std::size_t TilePlacement::hashCode() const {
	std::size_t hash = 0;
	hashcodes::combine(hash, hashcodes::collectionPrimitiveElements(tile().lowerId()));
	hashcodes::combine(hash, std::hash<BlockCuboid>{}(bounds()));
	hashcodes::combine(hash, hashcodes::primitive(placement().rotation));
	hashcodes::combine(hash, hashcodes::collectionHashCodeElements(mChildren));
	return hash;
}

DoorDef TilePlacement::transformedDoor(const TilePlacement& tp, const DoorDef& door) {
	return transformed(door, tp.blockPosTransform());
}

const PlacementVector TilePlacement::NoChildren;

const PlacementVector& TilePlacement::children() const {
	return mChildren;
}

void TilePlacement::_buildTransform() {
	auto tileSize = tile().size();
	auto& rotationTransform = BlockRegionTransform::GetRotate(mPlacement.rotation);
	mBlockPosTransform = blockpostransform::create(tileSize, rotationTransform, mPlacement.position);
	mBounds = BlockCuboid::fromPositionAndSize(mPlacement.position, rotationTransform.transformSize(tileSize));
	mInnerBounds = shrinked(mBounds, BlockPos(1, 0, 1));
}

//
// Free function intersection tests
//
bool intersects(const PlacementVector& existing, BlockCuboid bounds) {
	return std::any_of(existing.rbegin(), existing.rend(), [bounds](const auto& tp) { return tp.bounds().intersects(bounds); });
}

bool intersects(const PlacementVector& existing, const TilePlacement& placement) {
	return intersects(existing, placement.bounds());
}

bool intersectsXZ(const PlacementVector& existing, BlockCuboid bounds) {
	return std::any_of(existing.rbegin(), existing.rend(), [bounds](const auto& tp) { return tp.bounds().intersectsXZ(bounds); });
}

bool intersectsXZ(const PlacementVector& existing, const TilePlacement& placement) {
	return intersectsXZ(existing, placement.bounds());
}

BlockCuboid calculateBounds(const PlacementVector& placements) {
	BlockCuboid bounds;
	for (auto&& placement : placements) {
		safeExpand(bounds, placement.bounds());
	}
	return bounds;
}
