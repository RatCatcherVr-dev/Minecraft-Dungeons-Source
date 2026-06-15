#pragma once

#include <vector>

#include "NameTypes.h"
#include "modules/LiveOps/RewardResponse.h"
#include "modules/LiveOps/SeasonResponse.h"
#include "UnrealString.h"

namespace online
{
namespace liveops
{

class ChallengesHandler;
class RewardsHandler;
class ProgressHandler;

struct ChallengeRewardView {
	FName ItemNameId;
	minecraft::api::RewardTypes Type;
	minecraft::api::RewardState State;
};

struct ChallengeView {
	int CurrentScore;
	int RequiredScore;
	FText Description;
	FString Name;
	ChallengeRewardView Reward;
};

struct ChallengesView {
	std::vector<ChallengeView> Challenges;
};

class ChallengesViewBuilder {
public:
	static ChallengesView Build(ChallengesHandler* challengesHandler, RewardsHandler* rewardsHandler, ProgressHandler* progressHandler);
};

}
}
