#pragma once

#include "TerrainPos.h"
#include "Terrain.h"

namespace pathfinder {
	std::vector<TerrainPos> trace(const Terrain& terrain, TerrainPos start, TerrainPos end);
};