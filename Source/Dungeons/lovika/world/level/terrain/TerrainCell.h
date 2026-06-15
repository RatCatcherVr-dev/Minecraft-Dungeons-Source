#pragma once

class DUNGEONS_API TerrainCell {
public:	
	TerrainCell(char data = 0) : data { data } {}

	bool isReachable() const {
		return data == 0 || data == 3;
	}

	bool isKillzone() const {
		return data == 2 || data == 4;
	}

	bool isBelow() const {
		return data == 3 || data == 4;
	}

	bool isBoundary() const {
		return data == 1;
	}	

	static const TerrainCell unset;
	static const TerrainCell boundary;
	static const TerrainCell killzone;
	static const TerrainCell reachableBelow;
	static const TerrainCell killzoneBelow;

	char data;
};