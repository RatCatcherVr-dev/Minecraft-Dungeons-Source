#include "Dungeons.h"
#include "TrialUtil.h"
#include "game/LevelSettings.h"
#include "game/affector/AffectorTypes.h"
#include "game/difficulty/ExtraChallenge.h"
#include "game/item/generator/ItemGenerator.h"
#include "game/mission/MissionDefs.h"
#include "game/mission/MissionModifier.h"
#include "game/mission/MutableMissionDef.h"

namespace trial {

TArray<const MissionDef*> allTrials() {
	return missions::getAllMatching(RETLAMBDA(it.isTrial()));
}

//
// Apply/Prepare MissionDef objects with the trial data
//

void updateMissionDef(const FTrialDef& trial) {
	missions::overrides::Modify(false, [&](missions::MutableMissionDef& mission) {
		mission.setExtraChallenge(trial.extraChallenge);
		mission.appendAffectors(trial.affectors);
		mission.rewards(trial.rewards);
		if (trial.theme ) mission.setTheme(trial.theme.GetValue());
	})(trial.level);
}

void prepareMissionDefs(const TArray<FTrialDef>& trials) {
	missions::overrides::clear();

	algo::for_each(allTrials(), missions::overrides::Modify(true, [](auto& mission) { mission.disabled(); }));
	algo::for_each(trials, updateMissionDef);
}


}
