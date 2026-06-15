#include "LevelTransitionObjective.h"

#include "DungeonsGameInstance.h"
#include "online/sessions/OnlineUtil.h"
#include "online/seasons/LiveOps.h"
#include "online/seasons/Util/Filter.h"
#include "game/mission/variation/LevelVariationType.h"

namespace online
{
namespace liveops
{

LevelTransitionType::LevelTransitionType(ChallengeConfig config, minecraft::api::ObjectiveResponse objective)
	: Objective(config, objective) {
	CallbackHandle = online::getLiveOps(GameInstance)->OnMissionFinished.AddRaw(this, &LevelTransitionType::OnMissionFinished);
}
LevelTransitionType::~LevelTransitionType() {
	online::getLiveOps(GameInstance)->OnMissionFinished.Remove(CallbackHandle);
}

void LevelTransitionType::OnMissionFinished(ELevelNames name, EGameDifficulty difficulty, FEndlessStruggle endlessStruggle, EExtraChallenge extraChallenge, bool hyperMission, ELevelVariationType levelVariation, int collectedTokens, TOptional<FString> trialId) {
	if (filter::missioncompleted::isMatch(filter, name, difficulty, endlessStruggle, {trialId, Cast<UDungeonsGameInstance>(GameInstance)->GetTrialType(trialId.Get(""))})) {
		TriggerUpdate(1);
	}
}
}
}
