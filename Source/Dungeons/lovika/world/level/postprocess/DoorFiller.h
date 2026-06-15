#pragma once

#include "PostProcessConfigs.h"
#include "lovika/tile/TileDef.h"

class BlockSource;
class PlacedTiles;
class TilePlacement;
using PlacementVector = std::vector<TilePlacement>;

namespace postprocess { namespace door {

//void fillDoor(BlockSource&, const DoorDef&, const BlockProvider&, LevelGenRandom&);
std::vector<BlockCuboid> fillUnconnectedDoors(BlockSource&, const PlacedTiles& doorQuery, const PlacementVector&, const BlockProvider&);

}}
