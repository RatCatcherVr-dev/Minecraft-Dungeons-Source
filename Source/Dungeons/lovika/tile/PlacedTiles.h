#pragma once
#include "TilePlacement.h"

class BlockSource;
class TileGroup;

struct DoorPair {
	DoorDef from, to;
};

struct StretchId {
	StretchId(int index, int mainPathIndex = -1, bool isDefinitelyOffMainPath = false, int dungeonInstance = 0);

	int index;
	int mainPathIndex;
	bool isDefinitelyOffMainPath; // Hijacking the struct
	int dungeonInstance;
	//TOptional<std::string> offsiteReference;
};

StretchId copyAsOffMainPathStretchId(StretchId);

struct PlaceResult {
	TilePlacement tilePlacement;
	//StretchId stretchId;
	std::vector<DoorPair> doorPairs;

	std::size_t hashCode() const;
};
using PlaceResultVector = std::vector<PlaceResult>;

class PlacedTiles {
public:
	PlacedTiles() {}
	PlacedTiles(const PlaceResultVector&);

	BlockCuboid bounds() const;

	void add(PlaceResult);
	int pop_back(int count = 1);
	int pop_back_safe(int count = 1);

	bool isConnected(const DoorDef&) const;
	bool isOffsiteConnected(const DoorDef&) const;
	bool isSpatiallyConnected(const DoorDef&) const;

	// Collection interface
	size_t size() const;
	bool isEmpty() const;

	const TilePlacement& back() const;
	const TilePlacement& operator[](int index) const;
	PlacementVector::const_iterator begin() const;
	PlacementVector::const_iterator end() const;

	const PlaceResult& result(size_t index) const;
	const PlaceResultVector& results() const;

	const PlacementVector& placements() const;
	PlacementVector placementsIncludingChildren() const;
private:
	int _pop(int maxCount);
	void _recalculateBounds();

	PlaceResultVector mPlaced;
	PlacementVector mPlacements;
	std::unordered_set<BlockPos> mConnections;
	BlockCuboid mBounds;
};

void translate(PlaceResult&, BlockPos offset, bool translateEntryDoor = true, bool translateExitDoor = true);

std::size_t hashCode(const PlaceResultVector&);
