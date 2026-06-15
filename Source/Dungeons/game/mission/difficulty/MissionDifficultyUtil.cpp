#include "Dungeons.h"
#include "MissionDifficultyUtil.h"

bool UMissionDifficultyUtil::IsMissionDifficultyValid(const FMissionDifficulty& missionDifficulty) {
	return missionDifficulty.IsValid();
}
