#pragma once

#include "CoreMinimal.h"
#include "SeasonsObjective.h"
#include "game/difficulty/Difficulty.h"
#include "game/levels.h"
#include "game/mission/variation/LevelVariationType.h"
struct FMissionFinishedSummary;
class UGameInstance;

namespace online {
namespace liveops {

class LevelTransitionType : public Objective {

public:
	LevelTransitionType(ChallengeConfig, minecraft::api::ObjectiveResponse);
	virtual ~LevelTransitionType();

private:
	void OnMissionFinished(ELevelNames name,
							EGameDifficulty difficulty,
							FEndlessStruggle endlessStruggle,
							EExtraChallenge extraChallenge, 
							bool hyperMission,
							ELevelVariationType levelVariation, 
							int collectedTokens,
							TOptional<FString> trialId);
};
}
}
