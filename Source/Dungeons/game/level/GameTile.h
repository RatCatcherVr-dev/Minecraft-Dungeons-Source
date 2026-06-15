#pragma once

#include "ProgressInfo.h"
#include "GameDungeon.h"
#include "game/level/ambience/Ambience.h"
#include "game/mobspawn/SpawnRegions.h"
#include "lovika/BlockCuboid.h"
#include "lovika/tile/TileArea.h"
#include "lovika/world/level/levelgen/LevelDef.h"

enum class ELevelNames : uint8;

namespace game {

struct LevelDef;

class Tile {
public:
	Tile(const LevelDef&, unsigned int tileIndex);

	::TileRef tile() const;
	const MetaTile& meta() const;
	const Dungeon& dungeon() const;

	const TilePlacement& tilePlacement() const;
	const TileArea& tileArea() const;
	const BlockCuboid& bounds() const;

	const FString& ambienceGroupName() const;
	TOptional<EAmbienceID> ambience() const;
	TOptional<EAmbienceAudioID> ambienceAudio() const;
	TOptional<ELevelNames> musicLevelOverride() const;

	const generator::Stretch& stretch() const;

	io::Overrides overrides() const;

	bool isFurtherThan(const Tile& other) const;

	int tileDistanceToLevelEnd() const;

	bool isObjectiveTileHACK() const;

	const mobspawn::Regions& spawnRegions() const;

	const std::vector<lovika::Region>& immobileEnemyRegions() const; // D11.DB
	const ProgressInfo& progress() const;
private:
	generator::Tile mTile;
	TileArea mTileArea;
	levelgen::TileProgress mTileProgress;
	const LevelDef& mLevelDef;
	BlockCuboid mBounds;
	ProgressInfo mProgress;
	Dungeon mDungeon;
	mobspawn::Regions mSpawn;
	std::vector<lovika::Region> mImmobileEnemyRegions; // D11.DB
	io::Overrides mOverrides;

	TOptional<EAmbienceID> mAmbience;
	TOptional<EAmbienceAudioID> mAmbienceAudio;
	TOptional<ELevelNames> mMusicLevelOverride;
};

using TilePtr = const Tile*;
using TileRef = const Tile&;
}
