#include "Dungeons.h"
#include "PropPlacer.h"
#include "game/objective/TargetLoc.h"
#include "lovika/tile/TilePlacement.h"
#include "lovika/world/level/LevelGenRandom.h"
#include "util/FloatWeighedRandom.h"
#include "util/RandomUtil.h"

namespace decorator {

//
// Helper Util functions
//
bool fitsXZ(const BlockCuboid& bounds, BlockPos check, QuadrantAngle rotation) {
	return isPerpendicularlyRotated(rotation) ? bounds.fitsXZ(check.z, check.x) : bounds.fitsXZ(check);
}

Rotations getPossibleRotations(const BlockCuboid& area, MetaTileRef metaProp) {
	const auto size = metaProp.tile().size();
	return Rotations(algo::copy_if(metaProp.metadata.rotations.get(), RETLAMBDA(fitsXZ(area, size, it))));
}

BlockPos randomXZ(LevelGenRandom& rnd, BlockPos maxInc) {
	return{ rnd.nextInt(maxInc.x + 1), 0, rnd.nextInt(maxInc.z + 1) };
}

TOptional<Placement> randomPlacement(const Rotations& rotations, LevelGenRandom& rnd, const BlockCuboid& area, BlockPos size) {
	if (auto rotation = Util::randomChoiceOrEmpty(rotations.get(), &rnd)) {
		if (isPerpendicularlyRotated(rotation.GetValue())) {
			std::swap(size.x, size.z);
		}
		return Placement{ area.minInclusive + randomXZ(rnd, area.size() - BlockPos(size.x, 0, size.z)), rotation.GetValue()};
	}
	return {};
}


//
// PropPlacer
//
PropPlacer::PropPlacer(TileGroup repository, const std::vector<io::PropId>& pickFrom, LevelGenRandom& rnd, int targetArea, int maxFailStreak /*= 30*/)
	: mRepository(std::move(repository))
	, mPickFrom(pickFrom)
	, mRandom(rnd)
	, mAreaLeftToCover(targetArea)
	, mFailStreak(0)
	, mMaxFailStreak(maxFailStreak) {
}

void PropPlacer::tryPlaceOn(const std::string& stretch, TileRef tile, const lovika::Region& region, PlacementVector& placedProps) {
	auto it = mCache.find(region);
	if (it == mCache.end()) {
		it = mCache.emplace(region, _calculateCurrentProps(stretch, tile, region)).first;
	}
	// Filter all the "dynamic" properties here that we can't cache
	const auto regionPropCandidates = it->second.filter([this](MetaTileRef it) {
		return mFailStreak >= 4 || it.tile().size().productXz() >= 24;
	});
	if (auto pp = _getPlacableProp(regionPropCandidates, region, placedProps)) {
		mAreaLeftToCover -= pp->bounds().area();
		placedProps.push_back(pp.GetValue());
		mFailStreak = 0;
	} else {
		mFailStreak++;
	}
}

bool PropPlacer::isDone() const {
	return mPickFrom.empty() || mAreaLeftToCover <= 0 || mFailStreak >= mMaxFailStreak;
}

bool isRegionLocatorMatching(const io::RegionLocator& pattern, const TargetLoc& loc) {
	const auto patternLoc = TargetLoc::fromRegionLocator(pattern);
	return patternLoc && patternLoc->matches(loc);
}

MetaTileVector PropPlacer::_calculateCurrentProps(const std::string& stretch, TileRef tile, const lovika::Region& region) {
	const TargetLoc regionLoc{ Util::toLower(stretch), tile.lowerId(), region.lowerName() };

	MetaTileVector metaProps;
	for (auto& propId : mPickFrom) {
		auto metaProp = mRepository.findById(propId.lowerId);
		if (!metaProp) { // It's filtered out, e.g. based on the player's unlock keys
			continue;
		}
		if (!propId.targets.empty() && algo::none_of(propId.targets, RETLAMBDA(isRegionLocatorMatching(it, regionLoc)))) {
			continue;
		}
		if (!getPossibleRotations(region, *metaProp).get().empty()) {
			metaProps.push_back(*metaProp);
		}
	}
	return metaProps;
}

TOptional<TilePlacement> PropPlacer::_getPlacableProp(const TileGroup& currentProps, const lovika::Region& region, const PlacementVector& obstacles) const {
	if (!currentProps) {
		return {};
	}
	for (int tries = 0; tries < 30; ++tries) {
		auto& metaProp = *random(currentProps, mRandom);
		auto& prop = metaProp.tile();
		if (auto placement = randomPlacement(getPossibleRotations(region, metaProp), mRandom, region, prop.size())) {
			// Align it to the base y level (set up in the editor) correctly
			placement->position.y -= prop.baseY();
			TilePlacement tilePlacement(metaProp, placement.GetValue());
			if (!intersectsXZ(obstacles, tilePlacement)) {
				return tilePlacement;
			}
		}
	}
	return {};
}

}
