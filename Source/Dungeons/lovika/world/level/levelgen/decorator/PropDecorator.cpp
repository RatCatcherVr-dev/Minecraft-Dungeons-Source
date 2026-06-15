#include "Dungeons.h"
#include "PropDecorator.h"
#include "lovika/world/level/LevelGenRandom.h"
#include "lovika/world/level/levelgen/TileGroup.h"
#include "lovika/RegionPredicates.h"
#include "util/RandomUtil.h"
#include "PropPlacer.h"

namespace decorator {

PropDecorator::PropDecorator(TileGroup props)
	: mProps(std::move(props)) {
}

static int calculateTargetPropsArea(const std::vector<lovika::Region>& regions, State state) {
	const float PropDensityMultiplier = 0.5f;
	const int availablePropsArea = algo::sum(regions, RETLAMBDA(it.area().area()));
	return Math::round(availablePropsArea * Math::clamp(PropDensityMultiplier * state.decorDensity));
}

void PropDecorator::decorate(State state) const {
	if (state.decorGroups.empty()) {
		return;
	}
	const auto regions = state.tp.filterRegions(regionpredicates::type(regiontype::PropArea));
	PropPlacer placer(mProps, state.decorGroups, state.rnd, calculateTargetPropsArea(regions, state), 30);

	for (auto& region : algo::random::shuffledCopy(regions, state.rnd)) {
		if (placer.isDone()) {
			break;
		}
		placer.tryPlaceOn(state.stretchId, state.tp.tile(), region, state.decors);
	}
	while (!placer.isDone()) {
		placer.tryPlaceOn(state.stretchId, state.tp.tile(), *Util::randomChoice(regions, &state.rnd), state.decors);
	}
}

}
