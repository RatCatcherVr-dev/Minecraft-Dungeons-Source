#include "Dungeons.h"
#include "HyperMissionUtil.h"
#include "game/level/GameTiles.h"
#include "util/Algo.h"

namespace game { namespace util {

std::set<FString> getUniqueSubMissionIds(const game::Tiles& tiles) {
	return algo::map_as<std::set<FString>>(tiles.getSubDungeonInfos(), RETLAMBDA(it.dungeon.def().level.id));
}

}}

namespace levelgen { namespace hajper { namespace ids {

const CaseInsensitiveId& HyperDungeon() {
	static const CaseInsensitiveId id("@hyperdungeon");
	return id;
}

const CaseInsensitiveId& HyperLevel() {
	static const CaseInsensitiveId id("@hyperlevel");
	return id;
}

}}}
