#include "Dungeons.h"
#include "Challenges.h"
#include "ObjectiveTypes/SeasonsObjective.h"
#include "DungeonsGameState.h"
#include "ProgressHandler.h"
#include "util/DateTimeUtil.h"
#include "online/seasons/Util/SeasonsCommon.h"
#include "online/sessions/OnlineUtil.h"

namespace online
{
namespace liveops
{

namespace internal
{

IsChallengeActive GetChallengeActiveFunctionHandle(const minecraft::api::ChallengeResponse& data) {
	FDateTime startUTC = ToFDateTime(data.activeFrom);
	FDateTime endUTC = ToFDateTime(data.activeTo);
	return [startUTC, endUTC]() { return DateTimeUtil::InDateTimeWindow(startUTC, endUTC); };
}

}

Challenge::Challenge(ChallengeConfig config, minecraft::api::ChallengeResponse challengeData) :
	config(config), data(challengeData) {
	for (const auto& objectiveData : challengeData.objectives) {
		objectives.push_back(MakeObjective(config, objectiveData));
	}
}

const std::string& Challenge::GetName() const {
	return data.name;
}

bool Challenge::IsActive() const {
	return config.isActive();
}

bool Challenge::IsCompleted() const {
	if (auto progressHandler = online::getProgressHandlerInterface(config.GameInstance)) {
		return std::all_of(data.objectives.begin(), data.objectives.end(), [&](const minecraft::api::ObjectiveResponse& objective) {
			return progressHandler->GetStatValue(objective.progressName).Get(0) >= objective.requiredScore;
		});
	}
	return false;
}
const minecraft::api::ChallengeResponse& Challenge::GetData() const {
	return data;
}

Challenge* Challenges::Add(ChallengeConfig config, const minecraft::api::ChallengeResponse data) {
	config.isActive = internal::GetChallengeActiveFunctionHandle(data);
	challenges.emplace_back(std::make_unique<Challenge>(config, data));
	return challenges.back().get();
}

}
}
