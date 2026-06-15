#include "Dungeons.h"
#include "GameTiles.h"
#include "GameLevelDef.h"
#include "game/Conversion.h"
#include "lovika/tile/TilePlacement.h"
#include "lovika/RegionPredicates.h"
#include "lovika/Region.h"
#include "lovika/world/level/levelgen/generator/GraphUtil.h"
#include "world/phys/AABB.h"
#include "util/Algo.h"
#include "util/CollectionUtils.h"

static TOptional<BlockPos> getGround(const BlockRegion& blocks, int localX, int localZ) {
	BlockPos p(localX, blocks.size().y - 1, localZ);
	for (; p.y >= 0; --p.y) {
		if (blocks.getBlockId(p) != BlockID::AIR) {
			return p.above();
		}
	}
	return {};
}

namespace game {

int dungeonInstanceToIndex(const Dungeon& def) {
	// @note: We have this for now, since I really want dungeon instance ids
	// to start from 0, but I saw some issues with just changing one value
	return def.instanceId() - generator::MainDungeonInstanceId;
}

Tiles::Tiles(const LevelDef& levelDef)
	: mLevelDef(levelDef)
	, mTileGrid(TileCell{0})
{
	auto numTiles = levelDef.levelDef.tiles.size();
	checkf(numTiles == levelDef.tileAreas.size(), TEXT("TileArea vector isn't created!"));

	mTiles.reserve(numTiles);
	mTilePtrs.reserve(numTiles);
	for (auto i = 0u; i < numTiles; ++i) {
		mTiles.push_back(make_unique<Tile>(mLevelDef, i));
		mTilePtrs.push_back(mTiles.back().get());
	}
	mSubDungeonInfos = calculateSubDungeonGroups(getTiles());
	std::sort(mSubDungeonInfos.begin(), mSubDungeonInfos.end(), [](const auto& a, const auto& b) { return a.dungeon.instanceId() < b.dungeon.instanceId(); });
	check(algo::all_of(mSubDungeonInfos, RETLAMBDA(Util::isValidIndex(mSubDungeonInfos, dungeonInstanceToIndex(it.dungeon)))));

	checkf(mTilePtrs.size() < std::numeric_limits<TileCell::IndexType>::max(), TEXT("Too many tiles to fit in TileCell::IndexType!"));
	for (auto i = 0u; i < mTilePtrs.size(); ++i) {
		TileRef& tile = *mTilePtrs[i];

		for (auto posXz : flatten(tile.bounds())) {
			mTileGrid.set(posXz.x, posXz.z, { static_cast<TileCell::IndexType>(i + 1) });
		}
	}
}

static TOptional<BlockPos> getLocalStartPos(const TilePlacement& placement) {
	const auto name = placement.tile().lowerId();
	const auto blocks = placement.tile().blocks();
	const auto lc = blocks.size() / 2;
	return getGround(blocks, lc.x, lc.z);
}

FVector Tiles::getStartPos() const {
	return conversion::posToUe(centerFloor(getStartCuboid()));
}

FVector Tiles::getStartPos(TileRef tile) const {
	return conversion::posToUe(centerFloor(getStartCuboid(tile)));
}

BlockCuboid Tiles::getStartCuboid() const {
	std::vector<lovika::Region> startRegions;
	for (auto& tile : mTilePtrs) {
		const auto starts = tile->tilePlacement().filterRegions(regionpredicates::isPlayerStart());
		startRegions.insert(startRegions.end(), starts.begin(), starts.end());
	}
	//ensure(startRegions.size() <= 1);
	if (startRegions.size() >= 1) {
		return BlockCuboid(startRegions[0]);
	}
	return getStartCuboid(*mTiles[0]);
}

BlockCuboid Tiles::getStartCuboid(TileRef tile) const {
	const auto& placement = tile.tilePlacement();

	const auto startRegions = placement.filterRegions(regionpredicates::isPlayerStart());
	if (!startRegions.empty()) {
		return BlockCuboid(startRegions[0]);
	}
	const auto pos = getLocalStartPos(placement);
	checkf(pos, TEXT("Invalid start block!"));
	return BlockCuboid::fromPositionAndSize(placement.blockPosTransform()(pos.GetValue()), BlockPos(1));
}

TilePtr Tiles::getTile(BlockPos pos) const {
	if (const auto indexPlusOne = mTileGrid.get(pos.x, pos.z).tileIndexPlusOne) {
		return mTilePtrs[indexPlusOne - 1];
	}
	return nullptr;
}

bool Tiles::isSameTile(BlockPos a, BlockPos b) const {
	TilePtr tileA = getTile(a);
	return tileA && tileA->bounds().containsXZ(b); // Mostly because getTile() is slowish
}

TOptional<DoorToTile> Tiles::getPreviousDoor(TileRef tile) const {
	for (auto& c : generator::graph::util::allConnections(graph(), tileToNode(tile))) {
		const auto& neighbour = nodeToTile(c.other().node);
		const bool isPrev = tile.progress().isOnStrayPath() ?
			neighbour.progress().globalWithStrayPath().index() < tile.progress().globalWithStrayPath().index() :
			neighbour.progress().global().index() < tile.progress().global().index();
		if (isPrev) {
			return DoorToTile{ c.my().door, neighbour };
		}
	}
	return {};
}

TOptional<DoorToTile> Tiles::getPreviousDoor(BlockPos pos) const {
	auto tile = getTile(pos);
	return tile ? getPreviousDoor(*tile) : TOptional<DoorToTile> {};
}

bool hasDoorTag(const DoorDef& door, const std::string& lowerCasedTag) {
	return door.lowerTagString().find(lowerCasedTag) != std::string::npos;
}

TOptional<DoorToTile> Tiles::getNextDoor(TileRef tile) const {
	auto allConnections = generator::graph::util::allConnections(graph(), tileToNode(tile));
	for (auto& c : allConnections) {
		const auto& neighbour = nodeToTile(c.other().node);
		if (hasDoorTag(c.my().door, "path")) {
			return DoorToTile{ c.my().door, neighbour };
		}
	}
	for (auto& c : allConnections) {
		const auto& neighbour = nodeToTile(c.other().node);
		if ((!hasDoorTag(c.my().door, "fake") && neighbour.isFurtherThan(tile))) {
			return DoorToTile{ c.my().door, neighbour };
		}
	}
	return {};
}

TOptional<DoorToTile> Tiles::getNextDoor(BlockPos pos) const {
	auto tile = getTile(pos);
	return tile ? getNextDoor(*tile) : TOptional<DoorToTile>{};
}

TOptional<DoorToTile> Tiles::getRespawnDoor(TileRef tile) const {
	return tile.progress().isOnStrayPath() ? getNextDoor(tile) : getPreviousDoor(tile);
}

TOptional<DoorToTile> Tiles::getRespawnDoor(BlockPos pos) const {
	auto tile = getTile(pos);
	return tile ? getRespawnDoor(*tile) : TOptional<DoorToTile>{};
}

TOptional<game::DoorToTile> Tiles::getNextDoorIndicatorDoor(TileRef tile) const {
	// If we're on a side path and have no "previous" door (further into the sidepath),
	// it means we're in a dead end. If so, we should try to return the door indicator
	// of our "next" tile (closer to the main path) instead.
	// Note: I don't dare calling this 1-step recursively
	if (tile.progress().isOnStrayPath() && !getPreviousDoor(tile)) {
		if (auto next = getNextDoor(tile)) {
			if (tile.dungeon() == next->tile.dungeon()) {
				if (auto nextsDoorIndicator = getNextDoor(next->tile)) {
					return nextsDoorIndicator;
				}
			}
		}
	}
	return getNextDoor(tile);
}

TOptional<DoorToTile> Tiles::findDoorBetween(TileRef from, TileRef to) const {
	const auto dstNode = tileToNode(to);
	const auto startNode = tileToNode(from);

	if (dstNode == startNode) {
		// Don't return a link from a tile to the same tile
		return {};
	}

	for (auto& c : generator::graph::util::allConnections(graph(), tileToNode(from))) {
		if (c.other().node == dstNode) {
			return DoorToTile{ c.my().door, to };
		}
	}
	return {};
}

std::vector<TilePtr> Tiles::getTiles(const AABB& bb) const {
	return getTiles(BlockCuboid::fromInclusiveCorners(BlockPos(bb.min), BlockPos(bb.max)));
}

std::vector<TilePtr> Tiles::getTiles(const BlockCuboid& bounds) const {
	std::vector<TilePtr> tiles;
	for (auto& tile : mTiles) {
		if (tile->bounds().intersectsXZ(bounds)) {
			tiles.push_back(tile.get());
		}
	}
	return tiles;
}

const std::vector<TilePtr>& Tiles::getTiles() const {
	return mTilePtrs;
}

std::vector<TilePtr> Tiles::getAllNeighbours(TileRef tile) const {
	std::vector<TilePtr> out;
	for (auto& neighbourIndex : generator::graph::util::allNeighbouringConnectedNodes(graph(), tileToNode(tile))) {
		out.push_back(&nodeToTile(neighbourIndex));
	}
	return out;
}

std::vector<TilePtr> Tiles::getAllNeighboursAndThis(TileRef tile) const {
	auto tiles = getAllNeighbours(tile);
	tiles.push_back(&tile);
	return tiles;
}

std::vector<TilePtr> Tiles::getSpatiallyAdjacentNeighbours(TileRef tile) const {
	std::vector<TilePtr> out;
	for (auto& neighbourIndex : generator::graph::util::spatiallyNeighbouringConnectedNodes(graph(), tileToNode(tile))) {
		out.push_back(&nodeToTile(neighbourIndex));
	}
	return out;
}

std::vector<TilePtr> Tiles::getSpatiallyAdjacentNeighboursAndThis(TileRef tile) const {
	auto tiles = getSpatiallyAdjacentNeighbours(tile);
	tiles.push_back(&tile);
	return tiles;
}

TOptional<FVector> Tiles::getPreviousDoorPosition(TileRef tile) const {
	if (const auto prev = getRespawnDoor(tile)) {
		return conversion::blockToUe(prev->door.position());
	}
	return {};
}

TOptional<FVector> Tiles::getNextDoorPosition(TileRef tile) const {
	if (const auto next = getNextDoor(tile)) {
		return conversion::blockToUe(next->door.position());
	}
	return {};
}

const SubDungeonInfo& Tiles::getSubDungeonInfo(const Dungeon& dungeon) const {
	return mSubDungeonInfos[dungeonInstanceToIndex(dungeon)];
}

const std::vector<SubDungeonInfo>& Tiles::getSubDungeonInfos() const {
	return mSubDungeonInfos;
}

const generator::graph::Graph& Tiles::graph() const {
	return mLevelDef.levelDef.graph;
}

int Tiles::tileToNode(TileRef tile) const {
	const auto i = algo::index_of(mTilePtrs, &tile);
	check(i.IsSet());
	// @todo @note: keeping old behavior for now, when updating algo::index_of
	// to return TOptional due to me not having time to verify all usages.
	// I've never seen this <check> trigger though, but we will possibly
	// work with this graph in the future -- then it's good to have it.
	return i.Get(-1);
}

TileRef Tiles::nodeToTile(int i) const {
	check(Util::isValidIndex(mTilePtrs, i));
	return *mTilePtrs[i];
}

TilePtr Tiles::getClosestTile(BlockPos pos) const {
	if (auto insideTile = getTile(pos)) {
		return insideTile;
	}
	return algo::min_element_by(mTilePtrs, RETLAMBDA(manhattanXzDistanceTo(it->bounds(), pos))).Get(nullptr);
}

TilePtr getTile(const Tiles& tiles, const lovika::Region& region) {
	return tiles.getTile(region.area().minInclusive);
}

Tiles::operator const std::vector<TilePtr>&() const {
	return mTilePtrs;
}

//
// Free functions
//
TOptional<game::DoorToTile> backtrackToMainPath(const Tiles& tiles, TileRef start) {
	TilePtr tile = &start;

	while (tile->progress().isOnStrayPath()) {
		const auto prev = tiles.getRespawnDoor(*tile);
		if (!prev) {
			UE_LOG(LogTemp, Error, TEXT("No previous door from a stray path tile was found: '%s' (backtracked from '%s')"), tile->tile().id().c_str(), start.tile().id().c_str());
			return {};
		}
		if (!prev->tile.progress().isOnStrayPath()) {
			return game::DoorToTile{ tiles.findDoorBetween(prev->tile, *tile)->door, *tile };
		}
		tile = &prev->tile;
	}
	return {};
}

}
