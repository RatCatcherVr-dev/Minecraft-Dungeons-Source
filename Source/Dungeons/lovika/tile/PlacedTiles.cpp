#include "Dungeons.h"
#include "PlacedTiles.h"
#include "lovika/BlockPosTransform.h"
#include "util/Algo.hpp"
#include <numeric>

//
// Place result structs
//
StretchId::StretchId(int index, int mainPathIndex/*= -1*/, bool isDefinitelyOffMainPath/*= false*/, int dungeonInstance /*= 0*/)
	: index(index)
	, mainPathIndex(mainPathIndex >= 0 ? mainPathIndex : index)
	, isDefinitelyOffMainPath(isDefinitelyOffMainPath)
	, dungeonInstance(dungeonInstance) {
}

StretchId copyAsOffMainPathStretchId(StretchId s) {
	s.isDefinitelyOffMainPath = true;
	return s;
}

std::size_t PlaceResult::hashCode() const {
	return tilePlacement.hashCode();
}

//
// PlacedTiles
//

PlacedTiles::PlacedTiles(const PlaceResultVector& placements) {
	for (auto& placement : placements) {
		add(placement);
	}
}

void PlacedTiles::add(PlaceResult placed) {
	const auto& placement = placed.tilePlacement;

	if (mPlacements.empty()) {
		mBounds = placement.bounds();
	} else {
		mBounds.expand(placement.bounds());
	}
	for (auto& door : placement.doors()) {
		mConnections.insert(door.position());
	}
	mPlacements.push_back(placement);
	mPlaced.push_back(std::move(placed));
}

bool PlacedTiles::isConnected(const DoorDef& door) const {
	return isSpatiallyConnected(door) || isOffsiteConnected(door);
}

bool PlacedTiles::isOffsiteConnected(const DoorDef& door) const {
	const auto p = door.position();
	return algo::any_of(mPlaced, RETLAMBDA(algo::any_of(it.doorPairs, RETLAMBDA((it.from.position() == p) ^ (it.to.position() == p)))));
}

bool PlacedTiles::isSpatiallyConnected(const DoorDef& door) const {
	return mConnections.count(door.neighbourPos()) > 0;
}

const std::vector<TilePlacement>& PlacedTiles::placements() const {
	return mPlacements;
}

PlacementVector PlacedTiles::placementsIncludingChildren() const {
	PlacementVector tiles;
	for (auto&& tilePlacement: mPlacements) {
		tiles.push_back(tilePlacement);
		tiles.insert(tiles.end(), tilePlacement.children().begin(), tilePlacement.children().end());
	}
	return tiles;
}

const PlaceResult& PlacedTiles::result(size_t index) const {
	return mPlaced[index];
}

const PlaceResultVector& PlacedTiles::results() const {
	return mPlaced;
}

BlockCuboid PlacedTiles::bounds() const {
	return mBounds;
}

size_t PlacedTiles::size() const {
	return mPlacements.size();
}

bool PlacedTiles::isEmpty() const {
	return mPlacements.empty();
}

const TilePlacement& PlacedTiles::back() const {
	return mPlacements.back();
}

const TilePlacement& PlacedTiles::operator[](int index) const {
	return mPlacements[index];
}

PlacementVector::const_iterator PlacedTiles::begin() const {
	return std::begin(mPlacements);
}

PlacementVector::const_iterator PlacedTiles::end() const {
	return std::end(mPlacements);
}

int PlacedTiles::pop_back(int count /* = 1 */) {
	checkf((int)size() >= count, TEXT("Can't pop from empty stack!"));
	return _pop(count);
}

int PlacedTiles::pop_back_safe(int count /*= 1*/) {
	return _pop(count);
}

int PlacedTiles::_pop(int maxCount) {
	const int size = (int) mPlacements.size();
	const int c = std::min(maxCount, size);

	for (int i = size - c; i < size; ++i) {
		for (auto& door : mPlacements[i].doors()) {
			mConnections.erase(door.position());
		}
	}
	mPlacements.erase(mPlacements.end() - c, mPlacements.end());
	mPlaced.erase(mPlaced.end() - c, mPlaced.end());
	_recalculateBounds();
	return c;
}

void PlacedTiles::_recalculateBounds() {
	if (isEmpty()) {
		mBounds = BlockCuboid();
	} else {
		mBounds = mPlacements[0].bounds();
		for (unsigned i = 1; i < mPlacements.size(); ++i) {
			mBounds.expand(mPlacements[i].bounds());
		}
	}
}

//
// Free function helpers
//
void translate(PlaceResult& p, BlockPos offset, bool translateEntryDoor /*= true*/, bool translateExitDoor /*= true*/) {
	const auto transform = blockpostransform::offset(offset);
	p.tilePlacement.translate(offset);
	for (auto& doorPair : p.doorPairs) { // @multi @placeresult
		if (translateEntryDoor) { doorPair.from = transformed(doorPair.from, transform); }
		if (translateExitDoor) { doorPair.to = transformed(doorPair.to, transform); }
	}
}

std::size_t hashCode(const PlaceResultVector& placements) {
	return std::accumulate(placements.begin(), placements.end(), std::size_t {0}, [](std::size_t hash, const PlaceResult& p) {
		return hash * 13 + p.hashCode();
	});
}
