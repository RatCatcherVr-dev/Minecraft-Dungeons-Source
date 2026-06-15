#include "Dungeons.h"
#include "TilePreparer.h"
#include "game/Game.h"
#include "game/event/TileEvents.h"

namespace game { namespace tile {

TilePreparer::TilePreparer(const Game& game, int neighbourDistance)
	: mGame(game)
	, mBaseSeed(game.settings().randomSeed)
{
	mGame.tileEvents().tileEnterNew([this, neighbourDistance](auto&& ts) {
		_iterateNeighboursAndTrigger(ts.tile, neighbourDistance);
	});
}

TilePreparer& TilePreparer::add(TilePreparationCallback callback) {
	mCallbacks.push_back(std::move(callback));
	return *this;
}

void TilePreparer::_iterateNeighboursAndTrigger(const Tile& center, int neighbourDistance) {
	_spawnFor(center);

	if (neighbourDistance <= 0) {
		return;
	}
	for (auto&& tile : mGame.tiles().getSpatiallyAdjacentNeighbours(center)) {
		_iterateNeighboursAndTrigger(*tile, neighbourDistance - 1);
	}
}

void TilePreparer::_spawnFor(const Tile& tile) {
	if (mVisitedTiles.Contains(&tile)) {
		return;
	}
	mVisitedTiles.Add(&tile);

	mRandom.setSeed(mBaseSeed + tile.bounds().minInclusive.hashCode());

	const TilePreparationState state {
		mRandom,
		tile
	};
	for (const auto& callback : mCallbacks) {
		callback(state);
	}
}

}}
