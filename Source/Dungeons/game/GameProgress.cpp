#include "Dungeons.h"
#include "GameProgress.h"
#include "Game.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/event/TileEvents.h"
#include "lovika/io/LevelFileCommonTypes.h"
#include "Containers/Map.h"
#include "util/StringUtil.h"

namespace game {

GameProgress::GameProgress(Game& game, const std::vector<CaseInsensitiveId>& levelIds)
	: mTiles(game.tiles())
	, mLowerLevelIds(algo::map_as<TSet<FString>>(levelIds, RETLAMBDA(stringutil::toFString(it.lowerId))))
	, mInitialLevelId(stringutil::toFString(levelIds.empty() ? "<None>" : levelIds.front().lowerId))
	, mStart(*mTiles.getTiles().front())
	, mFurthest({ nullptr, mTiles.getTiles().front() })
{
	//D11.PS - added game namespace conflict keyword
	game.tileEvents().tileEnter([this](const events::TileState& state) {
		// Current
		if (mPlayerCurrent.Contains(&state.player)) {
			mPlayerPrevious.Add(&state.player, mPlayerCurrent[&state.player]);
		}
		mPlayerCurrent.Add(&state.player, &state.tile);
		if (state.player.IsLocallyControlled()) {
			mCurrentFurthestLocalPlayer.Add(&state.player, &state.tile);
		}
		//
		// Furthest
		//
		if (state.tile.isFurtherThan(*mFurthest.second)) {
			mFurthest = { &state.player, &state.tile };
		}
		auto furthest = mPlayerFurthest.Find(&state.player);
		if (!furthest || state.tile.isFurtherThan(**furthest)) {
			mPlayerFurthest.Add(&state.player, &state.tile);
		}
		// Visited tiles
		const auto visitTile = [](LevelTiles& levelTiles, const Tile& tile) {
			levelTiles.FindOrAdd(tile.dungeon().def().level.id.ToLower()).Add(&tile);
		};
		visitTile(mVisitedLevels, state.tile);
		visitTile(mPlayerVisitedLevels.FindOrAdd(&state.player), state.tile);
	});
}

TileRef GameProgress::current(const APlayerCharacter& player) const {
	return _thisOrStartTile(mPlayerCurrent.Find(&player));
}

TileRef GameProgress::previous(const APlayerCharacter& player) const {
	return _thisOrStartTile(mPlayerPrevious.Find(&player));
}

TileRef GameProgress::furthest(const APlayerCharacter& player) const {
	return _thisOrStartTile(mPlayerFurthest.Find(&player));
}

TileRef GameProgress::furthest() const {
	return *mFurthest.second;
}

TOptional<PlayerTile> GameProgress::currentlyFurthest() const {
	return _furthest(mPlayerCurrent);
}

TOptional<game::PlayerTile> GameProgress::currentlyLocalFurthest() const {
	return _furthest(mCurrentFurthestLocalPlayer);
}

int GameProgress::hyperMissionVisitedLevelCount(const APlayerCharacter& player) const {
	if (const auto* visited = mPlayerVisitedLevels.Find(&player)) {
		return _hyperMissionVisitedLevelCount(*visited);
	}
	return 0;
}

int GameProgress::hyperMissionVisitedLevelCount() const {
	return _hyperMissionVisitedLevelCount(mVisitedLevels);
}

TileRef GameProgress::_thisOrStartTile(const Tile* const* tile) const {
	return (tile && *tile)? **tile : *mTiles.getTiles().front();
}

TOptional<PlayerTile> GameProgress::_furthest(const PlayerTileMap& map) {
	TOptional<PlayerTile> furthest;
	for (const auto& elem : map) {
		if (elem.Key == nullptr) {
			continue;
		}
		if (!furthest || elem.Value->isFurtherThan(furthest->tile)) {
			furthest = PlayerTile { elem.Key.Get(), *elem.Value };
		}
	}
	return furthest;
}

int GameProgress::_hyperMissionVisitedLevelCount(const LevelTiles& levelTiles) const {
	const auto isSubMissionConsideredConsumed = [this](const LevelTiles::ElementType& e) {
		if (!mLowerLevelIds.Contains(e.Key)) {
			return false;
		}
		const int visitedTilesCount = e.Value.Num();
		return visitedTilesCount >= ((e.Key == mInitialLevelId) ? 1 : 2);
	}; 
	return algo::count_if(levelTiles, isSubMissionConsideredConsumed);
}

}
