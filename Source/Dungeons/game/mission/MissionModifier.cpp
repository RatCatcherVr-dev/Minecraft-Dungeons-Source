#include "Dungeons.h"
#include "MissionModifier.h"
#include "MissionDef.h"
#include "MissionDefs.h"
#include "MutableMissionDef.h"

namespace missions { namespace overrides {

Modify::Modify(bool baseOnCurrent, std::function<void(MutableMissionDef&)> f)
	: f(std::move(f))
	, current(baseOnCurrent) {
}

void Modify::operator()(const MissionDef* mission) const {
	update(mission->level());
}

void Modify::operator()(ELevelNames level) const {
	update(level);
}

void Modify::update(ELevelNames level) const {
	MutableMissionDef mutableMission = current ? getCurrent(level) : getOriginal(level);
	f(mutableMission);
	add(std::make_unique<MutableMissionDef>(mutableMission));
}

}}
