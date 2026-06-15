#include "TargetLoc.h"
#include "game/level/GameTile.h"
#include "util/Algo.h"
//#include <xutility> //D11.PS - doesn't exist on console

namespace game {

template <typename T>
TargetFinder<T>::TargetFinder(std::vector<TilePtr> tiles, Provider provider, Predicate predicate)
	: mTiles(std::move(tiles))
	, mProvider(std::move(provider))
	, mPredicate(std::move(predicate)) {
}

template <typename T>
std::vector<T> TargetFinder<T>::get(const io::RegionLocator& locator) const {
	// @todo: cache this lookup
	auto tiles = getTiles(locator);
	if (tiles.empty()) {
		return {};
	}
	auto loc = TargetLoc::fromRegionLocator(locator);

	std::vector<T> matches;
	for (TilePtr tile : tiles) {
		for (const auto& candidate : mProvider(*tile)) {
			if (mPredicate(*tile, loc->region, candidate)) {
				matches.push_back(candidate);
			}
		}
	}
	return matches;
}

template <typename T>
std::vector<T> TargetFinder<T>::get() const {
	std::vector<T> targets;
	for (TilePtr tile : mTiles) {
		auto tileTargets = mProvider(*tile);
		targets.insert(end(targets), std::make_move_iterator(begin(tileTargets)), std::make_move_iterator(end(tileTargets)));
	}
	return targets;
}

template <typename T>
std::vector<T> TargetFinder<T>::get(const std::vector<io::RegionLocator>& locators) const {
	std::unordered_set<T> targetSet;
	for (auto&& locator : locators) {
		auto targets = get(locator);
		if (!targets.empty()) {
			targetSet.insert(std::make_move_iterator(begin(targets)), std::make_move_iterator(end(targets)));
		}
	}

	//D11.PS - PS4 does not like this D11.TODO - Have another look at this
	//return std::vector<T>(std::make_move_iterator(begin(targetSet)), std::make_move_iterator(end(targetSet)));
	return std::vector<T>(begin(targetSet), end(targetSet));
}

template <typename T>
TOptional<T> TargetFinder<T>::single(const io::RegionLocator& locator) const {
	auto targets = get(locator);
	return targets.size() == 1 ? targets[0] : TOptional<T>{};
}

template <typename T>
std::vector<TilePtr> TargetFinder<T>::getTiles(const io::RegionLocator& locator) const {
	// @todo: cache this lookup
	if (auto maybeLoc = TargetLoc::fromRegionLocator(locator)) {
		const auto loc = maybeLoc.GetValue();
		return algo::copy_if(mTiles, RETLAMBDA(matchesTileAndStretch(loc, *it)));
	}
	return {};
}

template <typename T>
TilePtr TargetFinder<T>::singleTile(const io::RegionLocator& locator) const {
	auto tiles = getTiles(locator);
	return tiles.size() == 1 ? tiles[0] : nullptr;
}

}
