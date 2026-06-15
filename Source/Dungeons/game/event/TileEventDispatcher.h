#pragma once

#include <set>
#include "game/level/GameTile.h"

class APlayerCharacter;

namespace generator {
struct Stretch;
}

namespace game {
class Tile;
class Tiles;

namespace events {
class TileEvents;

class TileEventDispatcher {
public:
	TileEventDispatcher(const Tiles&);
	~TileEventDispatcher();
	void update(const TArray<APlayerCharacter*>&);
	TileEvents& events() const;
private:
	void _updatePlayer(APlayerCharacter&);
	void _updateTile(APlayerCharacter&, const Tile* const);
	void _updateStretch(APlayerCharacter&, const generator::Stretch* const);
	bool _tileUpdateFirstVisit(const Tile& tile);
	bool _stretchUpdateFirstVisit(const generator::Stretch& tile);

	struct Last { const Tile* tile; const generator::Stretch* stretch; };

	Unique<TileEvents> mEvents;
	const Tiles& mTiles;
	TMap<APlayerCharacter*, Last> mLast;

	std::set<const Tile*> mVisitedTiles;
	std::set<int> mVisitedStretches;
};

}}
