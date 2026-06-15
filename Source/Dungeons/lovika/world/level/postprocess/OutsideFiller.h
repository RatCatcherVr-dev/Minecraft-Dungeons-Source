#pragma once

#include "PostProcessConfigs.h"
#include "lovika/tile/TileArea.h"

class BlockSource;
class PlacedTiles;
class TilePlacement;
using PlacementVector = std::vector<TilePlacement>;

namespace postprocess { namespace worldfill {

const int NumBorderBlocks = 30;

std::vector<TileArea> fill(BlockSource&, const PlacedTiles& doorQuery, const PlacementVector&, const BlockProvider&);

}}
