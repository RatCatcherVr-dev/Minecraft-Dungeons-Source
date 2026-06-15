#pragma once
#include "world/level/BlockPos.h"

struct TerrainPos {
	int x;
	int y;
	
	TerrainPos() : TerrainPos(0, 0) {}

	TerrainPos(int x, int y) : x { x }, y { y } {}
	
	bool operator<(const TerrainPos& other) const {
		return y == other.y ?
			x < other.x :
			y < other.y;
	}

	TerrainPos operator+(const TerrainPos& other) const {
		return TerrainPos(x + other.x, y + other.y);
	}	

	TerrainPos operator-(const TerrainPos& other) const {
		return TerrainPos(x - other.x, y - other.y);
	}

	bool inline operator==(const TerrainPos& other) const {
		return x == other.x && y == other.y;
	}

	bool inline operator!=(const TerrainPos& other) const {
		return x != other.x || y != other.y;
	}
};

namespace std {	
	template<>
	struct hash<TerrainPos> {		
		size_t operator()(const TerrainPos position) const {
			return position.y << 16 | position.x;
		}
	};
}
