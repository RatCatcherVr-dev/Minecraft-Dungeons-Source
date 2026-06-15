#include "Dungeons.h"
#include "RegionFinder.h"
#include "TargetLoc.h"
#include "game/level/GameTile.h"
#include "lovika/Region.h"
#include "util/Algo.h"

namespace game {
	
namespace regionfinders {

bool RegionFilter(TileRef tile, const std::string& target, const lovika::Region& region) {
	return TargetLoc::matches(Util::toLower(target), region.lowerName());
}

RegionFinder Default(std::vector<TilePtr> tiles) {
	return RegionFinder(std::move(tiles), RETLAMBDA(it.tilePlacement().regions()), RegionFilter);
}

RegionFinder DefaultWith(std::vector<TilePtr> tiles, std::vector<RegionProvider> regionAdders) {
	regionAdders.push_back(regionproviders::Default());
	return RegionProviders(std::move(tiles), regionAdders);
}

RegionFinder RegionProviders(std::vector<TilePtr> tiles, std::vector<RegionProvider> regionProviders) {
	const auto regionsProvider = [providers = std::move(regionProviders)](TileRef tile) {
		RegionProviderState state { tile };

		std::vector<lovika::Region> allRegions;
		for (const auto& provider : providers) {
			const auto& regions = provider(state);
			allRegions.insert(allRegions.end(), regions.begin(), regions.end());
		}
		return allRegions;
	};
	return RegionFinder(tiles, regionsProvider, RegionFilter);
}

}

namespace regionproviders {

RegionProvider Default() {
	return [](const RegionProviderState& state) -> std::vector<lovika::Region> {
		return state.tile.tilePlacement().regions();
	};
}

}

}
