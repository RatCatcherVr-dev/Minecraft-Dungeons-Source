#include "Dungeons.h"
#include "ProgressInfo.h"
#include "GameLevelDef.h"
#include "lovika/world/level/levelgen/LevelDef.h"

namespace game {

ProgressInfo::ProgressInfo(const LevelDef& level, const levelgen::TileProgress& tile)
	: mLocal(tile.stretchTileIndex, tile.stretch(level.levelDef).length)
	, mGlobal(tile.globalTileIndex, level.levelLength)
	, mGlobalWithStrayPath(tile.globalTileIndex - tile.strayPathIndex, level.levelLength)
	, mStrayPath(tile.strayPathIndex, tile.strayPathLength)
	, mStretches(tile.stretchIndex, level.levelDef.stretches.size())
	, mStrayPathSubId(tile.strayPathSubId) {
}

const Progress& ProgressInfo::global() const {
	return mGlobal;
}

const Progress& ProgressInfo::globalWithStrayPath() const {
	return mGlobalWithStrayPath;
}

const Progress& ProgressInfo::strayPath() const {
	return mStrayPath;
}

const Progress& ProgressInfo::stretchLocal() const {
	return mLocal;
}

const game::Progress& ProgressInfo::stretches() const {
	return mStretches;
}

int ProgressInfo::strayPathSubId() const {
	return mStrayPathSubId;
}

bool ProgressInfo::mostlyEquals(const ProgressInfo& rhs) const {
	return mGlobal.index() == rhs.global().index() && mGlobalWithStrayPath.index() == rhs.globalWithStrayPath().index();
}

bool ProgressInfo::isOnStrayPath() const {
	return mStrayPath.index() > 0;
}

}
