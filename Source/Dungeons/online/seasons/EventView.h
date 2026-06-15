#pragma once

#include <vector>

#include "ChallengesView.h"
#include "DateTime.h"
#include "NameTypes.h"
#include "modules/LiveOps/RewardResponse.h"
#include "modules/LiveOps/SeasonResponse.h"

namespace online
{
namespace liveops
{

class SeasonsHandler;
class RewardsHandler;
class ProgressHandler;

struct EventRewardView {
	std::string RewardId;
	FName ItemNameId;
	minecraft::api::RewardTypes Type;
	minecraft::api::RewardState State;
};

struct EventLevelView {
	int RequiredAP;
	TOptional<EventRewardView> FreeReward;
	TOptional<EventRewardView> PaidReward;
};

struct EventView {
	FText Name;
	FDateTime StartDateTime;
	FDateTime EndDateTime;
	std::vector<EventLevelView> RewardLevels;
	ChallengesView ChallengeView;
};

class EventViewBuilder {
public:
	static TOptional<EventView> Build(SeasonsHandler*, RewardsHandler*, ProgressHandler*, ChallengesHandler*);
};

}
}
