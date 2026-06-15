#include "Dungeons.h"
#include "TileEventDispatcher.h"
#include "TileEvents.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/level/GameTiles.h"

namespace game { namespace events {

TileEventDispatcher::TileEventDispatcher(const Tiles& tiles)
	: mEvents(new events::TileEvents()) //D11.PS - added game namespace conflict keyword
	, mTiles(tiles)
{}

TileEventDispatcher::~TileEventDispatcher() {}

void TileEventDispatcher::update(const TArray<APlayerCharacter*>& players) {
	for (auto&& player : players) {
		_updatePlayer(*player);
	}
}

TileEvents& TileEventDispatcher::events() const {
	return *mEvents;
}

void TileEventDispatcher::_updatePlayer(APlayerCharacter& player) {
	auto tile = mTiles.getTile(player);
	_updateTile(player, tile);
	_updateStretch(player, tile ? &tile->stretch() : nullptr);
}

void TileEventDispatcher::_updateTile(APlayerCharacter& player, const Tile* const tile) {
	Last* last = mLast.Find(&player);
	if (last == nullptr) {
		last = &mLast.Add(&player);
	}

	if (tile == last->tile) {
		return;
	}

	if (last->tile) {
		mEvents->_dispatch(mEvents->mTileLeave, { *last->tile, player });
	}
	if (tile) {
		mEvents->_dispatch(mEvents->mTileEnter, { *tile, player });
	}

	bool isFirstVisit = tile && _tileUpdateFirstVisit(*tile);
	if (isFirstVisit) {
		mEvents->_dispatch(mEvents->mTileEnterNew, { *tile, player });
	}
	mEvents->_dispatch(mEvents->mTileTransition, { last->tile, tile, isFirstVisit, player });
	last->tile = tile;
}

void TileEventDispatcher::_updateStretch(APlayerCharacter& player, const generator::Stretch* const stretch) {
	Last* last = mLast.Find(&player);
	if (last == nullptr) {
		last = &mLast.Add(&player);
	}

	if (stretch == last->stretch) {
		return;
	}

	if (last->stretch) {
		mEvents->_dispatch(mEvents->mStretchLeave, { *last->stretch, player });
	}
	if (stretch) {
		mEvents->_dispatch(mEvents->mStretchEnter, { *stretch, player });
	}

	bool isFirstVisit = stretch && _stretchUpdateFirstVisit(*stretch);
	if (isFirstVisit) {
		mEvents->_dispatch(mEvents->mStretchEnterNew, { *stretch, player });
	}
	mEvents->_dispatch(mEvents->mStretchTransition, { last->stretch, stretch, isFirstVisit, player });
	last->stretch = stretch;
}

bool TileEventDispatcher::_tileUpdateFirstVisit(const Tile& tile) {
	if (mVisitedTiles.find(&tile) != end(mVisitedTiles)) {
		return false;
	}
	mVisitedTiles.insert(&tile);
	return true;
}

bool TileEventDispatcher::_stretchUpdateFirstVisit(const generator::Stretch& stretch) {
	if (mVisitedStretches.find(stretch.index) != end(mVisitedStretches)) {
		return false;
	}
	mVisitedStretches.insert(stretch.index);
	return true;
}

}}
