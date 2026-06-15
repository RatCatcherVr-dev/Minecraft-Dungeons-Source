#pragma once

#include <Optional.h>
#include <string>
#include <vector>

namespace io {
using RegionLocator = std::string;
}

namespace game {

class Tile;
class Tiles;
using TilePtr = const Tile*;
using TileRef = const Tile&;
	
template <typename T>
class TargetFinder {
	using Provider = std::function<std::vector<T>(TileRef)>;
	// @todo: PredicateProvider? so we easier can use regionpredicates namespace, more clever caching, etc
	using Predicate = std::function<bool(TileRef, const std::string&, const T&)>;
public:
	TargetFinder(std::vector<TilePtr>, Provider, Predicate);

	std::vector<T> get() const;
	std::vector<T> get(const io::RegionLocator&) const;
	std::vector<T> get(const std::vector<io::RegionLocator>&) const;
	TOptional<T> single(const io::RegionLocator&) const;

	std::vector<TilePtr> getTiles(const io::RegionLocator&) const;
	TilePtr singleTile(const io::RegionLocator&) const;
private:
	Provider mProvider;
	Predicate mPredicate;
	std::vector<TilePtr> mTiles;
};

}

#include "TargetFinder.hpp"
