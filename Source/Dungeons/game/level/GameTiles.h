#pragma once

#include "GameTile.h"
#include "GameLevelDef.h"

#include "game/actor/character/BaseCharacter.h"

struct BlockCuboid;
class Entity;
class AABB;

namespace lovika {
	class Region;
}

namespace generator { namespace graph { struct Graph; } }

namespace game {

struct DoorToTile {
	DoorDef door;
	TileRef tile;
};

class Tiles {
public:
	Tiles(const LevelDef&);

	FVector getStartPos() const;
	FVector getStartPos(TileRef) const;

	BlockCuboid getStartCuboid() const;
	BlockCuboid getStartCuboid(TileRef) const;

	// @todo: actually make these methods reasonably fast
	TilePtr getTile(BlockPos) const;
	bool isSameTile(BlockPos, BlockPos) const;
	TilePtr getClosestTile(BlockPos) const;

	TOptional<DoorToTile> getPreviousDoor(TileRef) const;
	TOptional<DoorToTile> getPreviousDoor(BlockPos) const;
	TOptional<DoorToTile> getNextDoor(TileRef) const;
	TOptional<DoorToTile> getNextDoor(BlockPos) const;
	TOptional<DoorToTile> getRespawnDoor(TileRef) const;
	TOptional<DoorToTile> getRespawnDoor(BlockPos) const;
	TOptional<DoorToTile> getNextDoorIndicatorDoor(TileRef) const;

	TOptional<DoorToTile> findDoorBetween(TileRef, TileRef) const;

	const std::vector<TilePtr>& getTiles() const;
	std::vector<TilePtr> getTiles(const BlockCuboid&) const;
	std::vector<TilePtr> getTiles(const AABB&) const;

	std::vector<TilePtr> getAllNeighbours(TileRef) const;
	std::vector<TilePtr> getAllNeighboursAndThis(TileRef) const;
	std::vector<TilePtr> getSpatiallyAdjacentNeighbours(TileRef) const;
	std::vector<TilePtr> getSpatiallyAdjacentNeighboursAndThis(TileRef) const;

	TOptional<FVector> getPreviousDoorPosition(TileRef) const;
	TOptional<FVector> getNextDoorPosition(TileRef) const;

	const SubDungeonInfo& getSubDungeonInfo(const Dungeon&) const;
	const std::vector<SubDungeonInfo>& getSubDungeonInfos() const;

	operator const std::vector<TilePtr>&() const;
private:
	const generator::graph::Graph& graph() const;
	int tileToNode(TileRef) const;
	TileRef nodeToTile(int) const;

	LevelDef mLevelDef;
	std::vector<Unique<Tile>> mTiles;
	std::vector<TilePtr> mTilePtrs;
	std::vector<SubDungeonInfo> mSubDungeonInfos;

	// @note: struct because we want to be clear in naming the "1 based index"
	//        and we could possibly cache more things later (e.g. dungeon index)
	struct TileCell { using IndexType = uint16_t; IndexType tileIndexPlusOne; };
	TerrainGrid<TileCell> mTileGrid;
};

TilePtr getTile(const Tiles&, const lovika::Region&);
TOptional<DoorToTile> backtrackToMainPath(const Tiles&, TileRef);

}
