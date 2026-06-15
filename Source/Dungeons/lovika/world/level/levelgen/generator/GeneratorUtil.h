#pragma once

#include "Generator.h"
#include "lovika/tile/TilePlacement.h"

class BlockRegion;
class PlacedTiles;
class VolumesIntersector;

namespace generator { namespace util {

BlockCuboid bounds(const std::vector<Tile>&);
TOptional<size_t> findIntersectingIndex(const std::vector<Tile>&, BlockPos);

const io::Dungeon* getDungeonFromId(const std::vector<io::Dungeon>&, const CaseInsensitiveId&);
const io::Dungeon& initialDungeon(const io::Level&, LevelGenRandom&);

PlaceResultVector initialPlacements(const MetaTile&);
DoorVector freeDoors(const DoorVector&, const PlacedTiles& doorChecker);
PlaceResultVector neighbourPlacements(const DoorVector& exitDoorCandidates, const MetaTile&, const DoorVector& entryDoorCandidates, int maxWidthDifference, const PlacedTiles& doorChecker);
PlaceResultVector removeIntersecting(const VolumesIntersector&, PlaceResultVector candidates);
TOptional<PlaceResult> canBePlaced(const DoorDef& base, const DoorDef&, const MetaTile&, int maxWidthDifference = 9999);
PlaceResultVector filterIfSome(const PlaceResultVector&, const Pred<const PlaceResult&>&);
PlaceResultVector filterIfSome(const PlaceResultVector&, const Pred<const TilePlacement&>&);

bool matchesAny(const DoorDef&, const std::vector<std::string>& names);
bool isExitDoorFor(const DoorDef&, const MetaTile&);

// @note: exitDoorCandidates and nonExitDoors are actually not opposites
//        (no defined exit-door: all doors are "exit door candidates".
//        This is something we might want to clarify further by naming.)
DoorVector exitDoorCandidates(const MetaTile&, const DoorVector&);
DoorVector exitDoorCandidates(const TileGroup&, const TilePlacement&);
DoorVector nonExitDoors(const MetaTile&, const DoorVector&);

}}
