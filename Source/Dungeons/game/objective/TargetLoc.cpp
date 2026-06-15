#include "Dungeons.h"
#include "TargetLoc.h"
#include "game/level/GameTile.h"
#include <algorithm>
#include <cctype>

TOptional<TargetLoc> TargetLoc::fromRegionLocator(const io::RegionLocator& locator) {
	auto b = locator.find('.');
	auto e = locator.find('.', b + 1);
	if (b == std::string::npos) {
		return {};
	}
	if (e == std::string::npos) {
		e = locator.length();
	}
	return TargetLoc{
		locator.substr(0, b),
		locator.substr(b + 1, e - b - 1),
		e == locator.length() ? "*" : locator.substr(e + 1)
	};
}

TargetLoc::TargetLoc(std::string stretch, std::string tile, std::string region)
	: stretch(Util::toLower(std::move(stretch)))
	, tile(Util::toLower(std::move(tile)))
	, region(Util::toLower(std::move(region))) {
}

bool TargetLoc::matches(const io::RegionLocator& locator) const {
	const auto loc = fromRegionLocator(locator);
	return loc && matches(loc.GetValue());
}

bool TargetLoc::matches(const std::string& pattern, const std::string& text) {
	return FString(text.c_str()).MatchesWildcard(pattern.c_str());
}

bool TargetLoc::matches(const TargetLoc& loc) const {
	return matchesStretch(loc.stretch) && matchesTile(loc.tile) && matchesRegion(loc.region);
}

//
// TargetLoc Queries
//
namespace game {

bool matchesTileAndStretch(const TargetLoc& loc, TileRef tile) {
	return loc.matchesStretch(tile.stretch().def.id) &&
		(
			loc.matchesTile(tile.tile().originalLowerId()) ||
			loc.matchesTile(tile.meta().lowerId) ||
			loc.matchesTile(tile.tile().lowerId())
		);
}

}
