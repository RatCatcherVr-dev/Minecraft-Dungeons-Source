#pragma once

#include "lovika/BlockCuboid.h"
#include <Optional.h>

enum class EResourcePack : uint8;

namespace io { struct DungeonDef; }

class BlockGraphicsPack;

namespace game {

class Tile;
using TilePtr = const Tile*;

struct Dungeon {
	Dungeon(const io::DungeonDef*, int dungeonInstanceId);

	const io::DungeonDef& def() const;
	int instanceId() const;
	const BlockGraphicsPack& blockGraphicsPack(UWorld*) const;

	TOptional<EResourcePack> resourcePack() const;

	bool operator==(const Dungeon&) const;
private:
	const io::DungeonDef* mDef;
	int mInstanceId;
	TOptional<EResourcePack> mResourcePack;
};

struct SubDungeonInfo {
	Dungeon dungeon;
	BlockCuboid bounds;
	std::vector<TilePtr> tiles;
};

std::vector<SubDungeonInfo> calculateSubDungeonGroups(const std::vector<TilePtr>&);

}
