#pragma once

#include <functional>

#include "MissionDefs.h"
#include "hyper/DungeonEligibility.h"
#include "lovika/io/IoHyperTypes.h"

struct FEligibleDLC;
struct ItemArchetypeCounts;

namespace io { struct HyperDungeon;}

namespace game { namespace mission { namespace dlc {

using DungeonEligibilityPredicate = std::function<missions::hajper::DungeonEligibility(const io::HyperDungeon&)>;

DungeonEligibilityPredicate getDungeonEligibilityPredicate(const FEligibleDLC& eligibleDLCs, const ItemArchetypeCounts&);
missions::MissionPred isExcludedMissionPredicate(const FEligibleDLC& eligibleDLCs);

}}}
