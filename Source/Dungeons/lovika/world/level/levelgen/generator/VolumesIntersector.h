#pragma once

class BlockSource;
class TileGroup;

#include "lovika/BlockCuboid.h"

class VolumesIntersector {
public:
	VolumesIntersector(bool intersects2d);

	bool isEmpty() const;
	const BlockCuboid& bounds() const;

	void add(BlockCuboid);
	int pop_back(int count = 1);
	int pop_back_safe(int count = 1);

	bool operator()(const BlockCuboid&) const;
private:
	int _pop(int maxCount);
	int _size() const;

	std::vector<BlockCuboid> mBounds;
	std::vector<BlockCuboid> mPlaced;
	bool mCheckXz;
};
