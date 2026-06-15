#pragma once

namespace io {
using RegionLocator = std::string;
}

struct TargetLoc {
	TargetLoc(std::string stretch, std::string tile, std::string region);
	std::string stretch;
	std::string tile;
	std::string region;

	bool matchesStretch(const std::string& s) const { return matches(stretch, s); }
	bool matchesTile   (const std::string& s) const { return matches(tile, s); }
	bool matchesRegion (const std::string& s) const { return matches(region, s); }
	bool matches(const TargetLoc&) const;
	bool matches(const io::RegionLocator&) const;

	static bool matches(const std::string& pattern, const std::string& text);

	static TOptional<TargetLoc> fromRegionLocator(const io::RegionLocator&);
};

namespace game {
class Tile;
class Tiles;
using TilePtr = const Tile*;
using TileRef = const Tile&;

bool matchesTileAndStretch(const TargetLoc&, TileRef);
}
