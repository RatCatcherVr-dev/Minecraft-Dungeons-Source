#pragma once

#include "CommonTypes.h"
#include "GameTile.h"
#include <Set.h>

class Random;

namespace game {
class Game;

namespace tile {

struct TilePreparationState {
	Random& rnd;
	TileRef tile;
};
using TilePreparationCallback = std::function<void(TilePreparationState)>;

class TilePreparer {
public:
	TilePreparer(const Game&, int neighbourDistance = 2);

	TilePreparer& add(TilePreparationCallback);
private:
	void _iterateNeighboursAndTrigger(const Tile&, int neighbourDistance);
	void _spawnFor(const Tile&);

	const Game& mGame;
	const RandomSeed mBaseSeed;
	Random mRandom;
	std::vector<TilePreparationCallback> mCallbacks;
	TSet<TilePtr> mVisitedTiles;
};

}}
