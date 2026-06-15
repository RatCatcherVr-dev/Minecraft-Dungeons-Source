#pragma once

#include "Boundary.h"
#include "DoorDef.h"
#include "RegionCell.h"
#include "WalkableCell.h"
#include "lovika/BlockRegion.h"
#include "lovika/BlockCuboid.h"
#include "lovika/Region.h"
#include "lovika/Tags.h"
#include "lovika/io/IoPrefabTypes.h"
#include "lovika/world/level/terrain/TerrainGrid.h"
#include "world/Facing.h"
#include "world/level/BlockPos.h"
#include "world/level/block/Block.h"

class TileDef {
public:
	TileDef(
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
	);
	TileDef(const TileDef&) = delete;

	const std::string& id() const;
	const std::string& lowerId() const;

	const std::string& originalLowerId() const;

	const std::string& lowerObjectGroupId() const;

	bool isTile() const;
	bool isProp() const;

	BlockPos size() const;

	void setBaseY(int baseY);
	int baseY() const;

	const BlockRegion& blocks() const;
	const std::vector<DoorDef>& doors() const;
	const std::vector<lovika::Region>& regions() const;
	const std::vector<Boundary>& boundaries() const;
	const std::vector<RegionCell>& regionPlane() const;
	const std::vector<WalkableCell>& walkableCells() const;
	const TerrainGrid<WalkableHeight>& walkablePlane() const;
	TOptional<Height> lowestWalkableBlockY() const;

	const Tags& tags() const;

	void setPosHACK(const BlockPos& posHACK);
	const BlockPos& posHACK() const;
private:
	std::string mId;
	std::string mLowerId;
	std::string mOriginalLowerId;
	std::string mLowerObjectGroupId;
	int mBaseY;
	BlockRegion mBlocks;
	std::vector<DoorDef> mDoors;
	// more metadata here
	std::vector<lovika::Region> mRegions;
	std::vector<Boundary> mBoundaries;
	std::vector<RegionCell> mRegionPlane;
	std::vector<WalkableCell> mWalkableCells;
	TerrainGrid<WalkableHeight> mWalkablePlane { WalkableHeight::Min() };
	TOptional<Height> mLowestWalkableBlockY;
	Tags mTags;
	BlockPos mPosHACK;
};

using TilePtr = const TileDef*;
using TileRef = const TileDef&;
using TileVector = std::vector<TilePtr>;
using TilePredicate = Pred<TileRef>;
using TileGetter = std::function<TilePtr()>;
using TileSelector = std::function<TilePtr(TileVector)>;

struct MetaTile: public io::WeightedTileId {
	explicit MetaTile(TileRef tile)
		: io::WeightedTileId(tile.id())
		, mTile(&tile) {}

	MetaTile(const io::WeightedTileId& id, TileRef tile, io::Tile::Metadata metadata = {})
		: io::WeightedTileId(std::move(id))
		, metadata(std::move(metadata))
		, mTile(&tile)
		 {}

	TileRef tile() const { return *mTile; }

	io::Tile::Metadata metadata;
private:
	TilePtr mTile;
};

using MetaTilePtr = const MetaTile*;
using MetaTileRef = const MetaTile&;
using MetaTileVector = std::vector<MetaTile>;
using MetaTilePredicate = Pred<MetaTileRef>;
