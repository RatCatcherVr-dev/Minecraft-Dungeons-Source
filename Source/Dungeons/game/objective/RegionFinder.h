#pragma once

#include "TargetFinder.h"

namespace lovika {
	class Region;
};

namespace game {

class Tile;
class Tiles;
using TilePtr = const Tile*;
using TileRef = const Tile&;

using RegionFinder = TargetFinder<lovika::Region>;

struct RegionProviderState { TileRef tile; };
using  RegionProvider = std::function<std::vector<lovika::Region>(const RegionProviderState&)>;

namespace regionfinders {

RegionFinder Default(std::vector<TilePtr>);
RegionFinder DefaultWith(std::vector<TilePtr>, std::vector<RegionProvider>);
RegionFinder RegionProviders(std::vector<TilePtr>, std::vector<RegionProvider>);

}

namespace regionproviders {

RegionProvider Default();

}

}
