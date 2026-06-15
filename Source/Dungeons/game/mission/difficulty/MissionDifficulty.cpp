#include "Dungeons.h"
#include "MissionDifficulty.h"
#include "game/mission/MissionDefs.h"
#include "game/mission/MissionSelection.h"
#include "game/component/MissionProgressComponent.h"

int FMissionDifficulty::compare(const FMissionDifficulty& rhs) const {
	if (int d = static_cast<int>(difficulty) - static_cast<int>(rhs.difficulty)) {
		return d;
	}
	if (int t = static_cast<int>(threatLevel) - static_cast<int>(rhs.threatLevel)) {
		return t;
	}
	if( int e = endlessStruggle.Value - rhs.endlessStruggle.Value ) {
		return e;
	}
	return 0;
}

FMissionDifficulty FMissionDifficulty::from(const FMissionProgress& progress) {
	return FMissionDifficulty{
		progress.levelName,
		progress.completedDifficulty,
		progress.completedThreatLevel,
		progress.completedEndlessStruggle
	};
}

FMissionDifficulty FMissionDifficulty::from(const FMissionSelection& selection) {
	return FMissionDifficulty{
		selection.levelName,
		selection.difficulty,
		selection.threatLevel,
		selection.endlessStruggle,
	};
}

bool FMissionDifficulty::IsValid() const {
	return mission != ELevelNames::Invalid && threatLevel != EThreatLevel::Invalid && difficulty != EGameDifficulty::Invalid;
}

FMissionDifficulty::operator game::FDifficulty() const {
	return { difficulty, threatLevel, missions::get(mission).getExtraChallenge(), endlessStruggle };
}
