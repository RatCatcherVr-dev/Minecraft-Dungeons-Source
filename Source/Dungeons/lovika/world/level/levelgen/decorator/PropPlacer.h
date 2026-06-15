#pragma once

#include "lovika/io/IoPrefabTypes.h"
#include "lovika/tile/TilePlacement.h"
#include "lovika/world/level/LevelGenRandom.h"
#include "lovika/world/level/levelgen/TileGroup.h"
#include <Optional.h>

namespace lovika { class Region; }

namespace decorator {

struct PropPlacer {
	PropPlacer(TileGroup repository, const std::vector<io::PropId>& pickFrom, LevelGenRandom&, int targetArea, int maxFailStreak = 30);

	void tryPlaceOn(const std::string& stretch, TileRef, const lovika::Region&, PlacementVector& placedProps);

	bool isDone() const;
private:
	MetaTileVector _calculateCurrentProps(const std::string& stretch, TileRef, const lovika::Region&);
	TOptional<TilePlacement> _getPlacableProp(const TileGroup&, const lovika::Region&, const PlacementVector& obstacles) const;

	TileGroup mRepository;
	const std::vector<io::PropId>& mPickFrom;
	LevelGenRandom& mRandom;

	int mFailStreak;
	int mMaxFailStreak;
	int mAreaLeftToCover;

	std::unordered_map<lovika::Region, TileGroup> mCache;
};

}
