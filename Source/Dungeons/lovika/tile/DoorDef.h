#pragma once

#include "world/level/BlockPos.h"
#include "world/Facing.h"
#include "lovika/BlockCuboid.h"

class DoorDef {
public:
	DoorDef(const BlockPos& position, FacingID, int width, const std::string& name, const std::string& tagString, TOptional<FString> prefab = {});

	BlockPos position() const;
	BlockPos neighbourPos() const;

	int width() const;

	Facing::Name facing() const;
	Facing::Name neighbourFacing() const;

	BlockCuboid positions() const;
	std::pair<BlockPos, BlockPos> doorFrame() const;

	bool isSpatiallyAdjacent(const DoorDef&) const;

	const std::string& name() const;
	bool nameMatches(const std::string&) const;

	const std::string& lowerTagString() const;

	TOptional<FString> prefab; // @temporary: teleport prefabs

	bool operator==(const DoorDef&) const;
	bool operator!=(const DoorDef&) const;
private:
	std::string mName;
	std::string mLowerName;

	std::string mLowerTagString;

	BlockPos mPosition;
	BlockPos mNeighbourPosition;
	int mWidth;

	Facing::Name mFacing;
	Facing::Name mNeighbourFacing;
};

using DoorVector = std::vector<DoorDef>;

DoorDef              transformed(const DoorDef&, const BlockPosTransform&);
std::vector<DoorDef> transformed(const std::vector<DoorDef>&, const BlockPosTransform&);
