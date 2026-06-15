#include "MissionDLCUtil.h"
#include "dlc/EligibleDLC.h"
#include <Anticheat.hpp>
#include "hyper/DungeonEligibility.h"
#include "lovika/io/IoHyperTypes.h"
#include "util/Algo.h"

namespace game { namespace mission { namespace dlc {


ANTICHEAT_NO_OPTIMIZATION_BEGIN

DungeonEligibilityPredicate getDungeonEligibilityPredicate(const FEligibleDLC& eligibleDLCs, const ItemArchetypeCounts& archetypeCounts) {
	ANTICHEAT_VIRT_BEGIN
	using namespace missions::hajper;
	return [eligibleDLCs,archetypeCounts] (const io::HyperDungeon& dungeon) {
			if (archetypeCounts.isLargerThan(dungeon.archetypeRequirements)) {
				if (auto requiredDLC = dungeon.requiredDLC) {
					if (!eligibleDLCs.IsEligible(requiredDLC.GetValue())) {
						return DungeonEligibility::COULD_PLAY_IF_YOU_HAD_DLC;
					}
				}
				return DungeonEligibility::CAN_PLAY_DUNGEON;
			}
			return DungeonEligibility::CANNOT_PLAY_DUNGEON;
		};
	ANTICHEAT_VIRT_END
}

missions::MissionPred isExcludedMissionPredicate(const FEligibleDLC& eligibleDLCs) {
	ANTICHEAT_VIRT_BEGIN
	return [
		eligibleDLCs
	] (const MissionDef& mission) {
		if (auto requiredDLC = mission.getRequiredDLC()) {
			return !eligibleDLCs.IsEligible(requiredDLC.GetValue());
		}
		return false;
	};
	ANTICHEAT_VIRT_END
}

ANTICHEAT_NO_OPTIMIZATION_END

}}}
