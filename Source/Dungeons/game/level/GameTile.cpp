#include "Dungeons.h"
#include "GameTile.h"
#include "GameLevelDef.h"
#include "game/levels.h"
#include "lovika/RegionPredicates.h"
#include "util/EnumUtil.h"

namespace game {

Tile::Tile(const LevelDef& level, unsigned int tileIndex)
	: mTile(level.levelDef.tiles[tileIndex])
	, mTileArea(level.tileAreas[tileIndex])
	, mTileProgress(level.levelDef.progress[tileIndex])
	, mLevelDef(level)
	, mBounds(tilePlacement().bounds())
	, mProgress(level, mTileProgress)
	, mDungeon{ mTileProgress.dungeon(level.levelDef), mTileProgress.stretch(level.levelDef).dungeonInstance }
	, mOverrides(mTile.overrides(level.levelDef))
	, mSpawn(tilePlacement().filterRegions(regionpredicates::isRegularSpawn()))
	, mImmobileEnemyRegions(tilePlacement().filterRegions(regionpredicates::isImmobileMob())) // D11.DB
	, mAmbience(EnumValueFromString(EAmbienceID, "ambience_" + mOverrides.ambience.Get("")))
	, mAmbienceAudio(EnumValueFromString(EAmbienceAudioID, mOverrides.audioAmbience.Get("")))
{
	if (const auto* dungeonDef = mTileProgress.dungeon(level.levelDef)) {
		if (dungeonDef->musicLevelOverride) {
			mMusicLevelOverride = GetEnumValueFromStringT<ELevelNames>(dungeonDef->musicLevelOverride.GetValue());
		}
	}
}

::TileRef Tile::tile() const {
	return mTile.placeResult.tilePlacement.tile();
}

const MetaTile& Tile::meta() const {
	return mTile.metaTile;
}

const Dungeon& Tile::dungeon() const {
	return mDungeon;
}

const TilePlacement& Tile::tilePlacement() const {
	return mTile.placeResult.tilePlacement;
}

const TileArea& Tile::tileArea() const {
	return mTileArea;
}

const BlockCuboid& Tile::bounds() const {
	return mBounds;
}

const FString& Tile::ambienceGroupName() const {
	return dungeon().def().level.ambienceLevelId;
}

TOptional<EAmbienceID> Tile::ambience() const {
	return mAmbience;
}

TOptional<EAmbienceAudioID> Tile::ambienceAudio() const {
	return mAmbienceAudio;
}

TOptional<ELevelNames> Tile::musicLevelOverride() const {
	return mMusicLevelOverride;
}

const generator::Stretch& Tile::stretch() const {
	return mTileProgress.stretch(mLevelDef.levelDef);
}

io::Overrides Tile::overrides() const {
	return mOverrides;
}

bool Tile::isFurtherThan(const Tile& other) const {
	return tileDistanceToLevelEnd() < other.tileDistanceToLevelEnd();
}

int Tile::tileDistanceToLevelEnd() const {
	return mLevelDef.levelLength - mTileProgress.globalTileIndex + mTileProgress.strayPathIndex;
}

bool Tile::isObjectiveTileHACK() const {
	return tilePlacement().filterRegions(regionpredicates::isObjectiveHACK()).size() >= 1;
}

const ProgressInfo& Tile::progress() const {
	return mProgress;
}

const mobspawn::Regions& Tile::spawnRegions() const {
	return mSpawn;
}

const std::vector<lovika::Region>& Tile::immobileEnemyRegions() const { // D11.DB
	return mImmobileEnemyRegions;
}

}
