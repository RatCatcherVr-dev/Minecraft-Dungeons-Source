#include "Dungeons.h"
#include "TerrainCell.h"

const TerrainCell TerrainCell::unset { 0 };
const TerrainCell TerrainCell::boundary { 1 };
const TerrainCell TerrainCell::killzone { 2 };
const TerrainCell TerrainCell::reachableBelow { 3 };
const TerrainCell TerrainCell::killzoneBelow { 4 };
