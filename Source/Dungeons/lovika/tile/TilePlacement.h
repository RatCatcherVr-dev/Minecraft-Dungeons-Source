#pragma once

class BlockSource;

#include "world/level/BlockPos.h"
#include "lovika/QuadrantAngle.h"
#include "lovika/BlockCuboid.h"
#include "lovika/Region.h"
#include "TileDef.h"
#include "Boundary.h"
#include "RegionCell.h"

class TilePlacement;
using PlacementVector = std::vector<TilePlacement>;
using SubObjects = PlacementVector;

struct Placement {
	BlockPos position;
	QuadrantAngle rotation;
	// @todo: flip?
};

class TilePlacement {
public:
	TilePlacement(const std::string& metaTileId, TileRef, Placement, TOptional<TilePlacement> originalPlacement = {});
	TilePlacement(const MetaTile&, Placement, TOptional<TilePlacement> originalPlacement = {});
	TilePlacement& operator=(const TilePlacement&) = default;
	~TilePlacement();

	TileRef tile() const;
	const std::string& tileIdHACK() const;
	const std::string& metaIdHACK() const; // HACK, because it's strictly not the best place for it (and we usually want a MetaTile, not its id)

	Placement placement() const;
	TOptional<TilePlacement> originalPlacement() const;
	const BlockCuboid& bounds() const;

	void translate(BlockPos offset);

	std::vector<DoorDef> doors() const;
	std::vector<lovika::Region> regions() const;
	std::vector<Boundary> boundaries() const;
	std::vector<RegionCell> regionPlane() const;
	std::vector<WalkableCell> walkableCells() const;
	std::vector<lovika::Region> filterRegions(const RegionPredicate&) const;
	bool hasRegion(const RegionPredicate&) const;
	TOptional<Height> lowestWalkableBlockY() const;

	bool isPotentialTeleportDoor(const DoorDef&) const;

	const BlockPosTransform& blockPosTransform() const;

	PlacementVector& children();
	const PlacementVector& children() const;

	std::size_t hashCode() const;

	static DoorDef transformedDoor(const TilePlacement&, const DoorDef&);
private:
	void _buildTransform();
	void _regions(std::vector<lovika::Region>& out) const;
	void _filterRegions(const RegionPredicate&, std::vector<lovika::Region>& out) const;

	std::string mMetaId;
	TilePtr mTile;
	Placement mPlacement;
	std::vector<TilePlacement> mOriginalPlacement; // HACK -- I'm lazy. Something like HeapOptional<T>? // Aron

	BlockCuboid mBounds;
	BlockCuboid mInnerBounds;
	BlockPosTransform mBlockPosTransform;

	PlacementVector mChildren;
	std::string mIdHACK;
	static const PlacementVector NoChildren;
};

bool intersects(const PlacementVector&, BlockCuboid);
bool intersects(const PlacementVector&, const TilePlacement&);
bool intersectsXZ(const PlacementVector&, BlockCuboid);
bool intersectsXZ(const PlacementVector&, const TilePlacement&);

BlockCuboid calculateBounds(const PlacementVector&);
