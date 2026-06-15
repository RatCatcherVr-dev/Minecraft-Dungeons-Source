#pragma once

#include <vector>

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

struct SeasonRewardView {
	std::string RewardId;
	FName ItemNameId;
	minecraft::api::RewardTypes Type;
	minecraft::api::RewardState State;
};

struct SeasonLevelView {
	int RequiredAP;
	TOptional<SeasonRewardView> FreeReward;
	TOptional<SeasonRewardView> PaidReward;
};

struct SeasonView {
	int Number;
	FText Name;
	FDateTime StartDateTime;
	FDateTime EndDateTime;
	int PlayerLevel;
	float PlayerLevelProgress;
	std::vector<SeasonLevelView> RewardLevels;
};

class SeasonViewBuilder {
public:
	static SeasonView Build(SeasonsHandler* seasonsHandler, RewardsHandler* rewardsHandler, ProgressHandler* progressHandler);
};

}
}
