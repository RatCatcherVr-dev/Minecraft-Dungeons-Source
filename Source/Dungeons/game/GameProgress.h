#pragma once

class APlayerCharacter;
struct CaseInsensitiveId;

namespace game {

class Game;
class Tile;
class Tiles;
using TileRef = const Tile&;

struct PlayerTile {
	APlayerCharacter* player;
	TileRef tile;
};

class GameProgress {
	using PlayerWeakPtr = TWeakObjectPtr<APlayerCharacter>;
	using PlayerTileMap = TMap<PlayerWeakPtr, const Tile*>;
	using LevelTiles = TMap<FString, TSet<const Tile*>>;
public:
	GameProgress(Game&, const std::vector<CaseInsensitiveId>& levelIds);

	TileRef current(const APlayerCharacter&) const;
	TileRef previous(const APlayerCharacter&) const;
	TileRef furthest(const APlayerCharacter&) const;

	TileRef furthest() const;
	TOptional<PlayerTile> currentlyFurthest() const;
	TOptional<PlayerTile> currentlyLocalFurthest() const;

	int hyperMissionVisitedLevelCount(const APlayerCharacter&) const;
	int hyperMissionVisitedLevelCount() const;
private:
	TileRef _thisOrStartTile(const Tile* const*) const;
	static TOptional<PlayerTile> _furthest(const PlayerTileMap&);
	int _hyperMissionVisitedLevelCount(const LevelTiles&) const;

	const Tiles& mTiles;
	TSet<FString> mLowerLevelIds;
	FString mInitialLevelId;
	PlayerTileMap mPlayerCurrent;
	PlayerTileMap mPlayerPrevious;
	PlayerTileMap mPlayerFurthest;
	PlayerTileMap mCurrentFurthestLocalPlayer;
	TMap<PlayerWeakPtr, LevelTiles> mPlayerVisitedLevels;
	LevelTiles mVisitedLevels;
	std::pair<PlayerWeakPtr, const Tile*> mFurthest;
	TileRef mStart;
};

}
