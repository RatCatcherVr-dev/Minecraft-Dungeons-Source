#include "Dungeons.h"
#include "GameDungeon.h"
#include "GameTile.h"
#include "client/resource/Resource.h"
#include "client/renderer/block/BlockGraphicsPack.h"
#include "util/Algo.hpp"
#include "util/EnumUtil.h"
#include "util/StringUtil.h"

namespace game {

//
// Dungeon
//
Dungeon::Dungeon(const io::DungeonDef* def, int dungeonInstanceId)
	: mDef(def)
	, mInstanceId(dungeonInstanceId)
	, mResourcePack(def ? GetEnumValueFromStringT<EResourcePack>(stringutil::toFString(def->resourcePack)) : TOptional<EResourcePack>{}) {
}

const io::DungeonDef& Dungeon::def() const {
	return *mDef;
}

int Dungeon::instanceId() const {
	return mInstanceId;
}

TOptional<EResourcePack> Dungeon::resourcePack() const {
	return mResourcePack;
}

bool Dungeon::operator==(const Dungeon& rhs) const {
	return mInstanceId == rhs.mInstanceId;
}

const BlockGraphicsPack& Dungeon::blockGraphicsPack(UWorld* world) const {
	return BlockGraphicsHelper::getInstance(world).get(ResourcePack(mDef->resourcePack.c_str()));
}

//
// SubDungeonGroup
//
std::vector<SubDungeonInfo> calculateSubDungeonGroups(const std::vector<TilePtr>& tiles) {
	std::vector<SubDungeonInfo> groups;

	const auto findOrAddDungeonFor = [&](TilePtr tile) -> SubDungeonInfo& {
		if (const auto i = algo::index_of_if(groups, RETLAMBDA(it.dungeon == tile->dungeon()))) {
			return groups[i.GetValue()];
		}
		groups.push_back({ tile->dungeon() });
		return groups.back();
	};

	for (auto* tile : tiles) {
		auto& group = findOrAddDungeonFor(tile);
		group.tiles.push_back(tile);
		safeExpand(group.bounds, tile->bounds());
	}
	return groups;
};

}
