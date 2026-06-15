#pragma once

#include "lovika/Grid2.h"

struct TileArea {
	uint32 full;
	uint32 floor;
	uint32 floorWalls;
	Grid2<Height> heights;
};
