#pragma once

#include "game/objective/TargetFinder.h"
#include "lovika/io/LevelFile.h"
#include "lovika/tile/DoorDef.h"
#include "lovika/world/level/levelgen/generator/Graph.h"

class TilePlacement;

namespace generator {

struct Result;

namespace alpha {

class GenState;

using TeleportDoorFinder = game::TargetFinder<DoorDef>;

struct TeleportDoorDef {
	io::Teleport def;
	DoorDef door;

	DoorDef merged() const;
};

TArray<TeleportDoorDef> getTeleportDoors(const TilePlacement&, const std::vector<io::Teleport>&, bool allowUnlistedTeleports);

TOptional<TeleportDoorDef> findTeleportDoor(const GenState&, const TeleportDoorFinder&, const io::RegionLocator&);

std::vector<std::vector<io::Stretch>> splitStretchesIntoJumpConnectedGroups(const TileGroup&, const std::vector<io::Stretch>&);
Result connectJumpDoorsAndMakeResult(const GenState&, std::vector<graph::Edge>);

}}
