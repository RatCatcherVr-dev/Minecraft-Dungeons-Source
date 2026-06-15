#include "Dungeons.h"

#include "SeasonsObjective.h"
#include "BlockTravelObjective.h"
#include "CurrencyCollectObjective.h"
#include "ChestOpenedObjective.h"
#include "HealObjective.h"
#include "KillObjective.h"
#include "LevelTransitionObjective.h"
#include "ReviveFriendObjective.h"
#include "XPObjectiveType.h"
#include "ItemUseObjective.h"
#include "modules/LiveOps/ChallengeResponse.h"
#include "online/seasons/LiveOps.h"
#include "online/sessions/OnlineUtil.h"

#include <AssertionMacros.h>

namespace online
{
namespace liveops
{


Objective::Objective(ChallengeConfig config, minecraft::api::ObjectiveResponse objective) :
	data(std::move(objective)),
	GameInstance(config.GameInstance),
	isActive(config.isActive) {
	for (const auto& part : data.filterParts) {
		filter.push_back({part.type, part.value});
	}
}

minecraft::api::ObjectiveType Objective::GetType() const {
	return data.type;
}

const std::string& Objective::GetProgressName() const {
	return data.progressName;
}

const minecraft::api::ObjectiveResponse& Objective::GetData() const {
	return data;
}

void Objective::TriggerUpdate(int64 score) const {
	if (auto progress = getProgressHandlerInterface(GameInstance)) {
		if (isActive()) {
			progress->AddToQueue(MakeProgressRequest(data.progressName, score));
		} else {
			UE_LOG(LogLiveOps, Log, TEXT("[LiveOps] TriggerUpdate - challenge not active, not storing progress. (%s)"), *FString(GetData().progressName.c_str()));
		}
	}
}

std::unique_ptr<Objective> MakeObjective(ChallengeConfig config, const minecraft::api::ObjectiveResponse& objective) {
	switch (objective.type) {
	case minecraft::api::ObjectiveType::MobDeath:
		return std::make_unique<KillObjective>(config, objective);
	case minecraft::api::ObjectiveType::XpGained:
		return std::make_unique<XPObjectiveType>(config, std::move(objective));
	case minecraft::api::ObjectiveType::MissionCompleted:
		return std::make_unique<LevelTransitionType>(config, std::move(objective));
	case minecraft::api::ObjectiveType::PlayerHealed:
		return std::make_unique<HealObjective>(config, std::move(objective));
	case minecraft::api::ObjectiveType::ReviveFriend:
		return std::make_unique<ReviveFriendObjective>(config, std::move(objective));
	case minecraft::api::ObjectiveType::CurrencyCollected:
		return std::make_unique<CurrencyCollectObjective>(config, std::move(objective));
	case minecraft::api::ObjectiveType::TrialComplete:
		return std::make_unique<LevelTransitionType>(config, std::move(objective));
	case minecraft::api::ObjectiveType::ItemUsed:
		return std::make_unique<ItemUseObjective>(config, std::move(objective));
	case minecraft::api::ObjectiveType::ChestOpened:
		return std::make_unique<ChestOpenedObjective>(config, std::move(objective));
	case minecraft::api::ObjectiveType::BlocksTravelled:
		return std::make_unique<BlockTravelObjective>(config, std::move(objective));
	default:
		checkNoEntry();
		return {};
	}
}


}
	}
