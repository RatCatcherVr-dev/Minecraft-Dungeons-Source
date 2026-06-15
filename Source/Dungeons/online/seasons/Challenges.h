#pragma once

#include "modules/LiveOps/ChallengeResponse.h"
#include "online/seasons/Rewards.h"
#include "online/seasons/ObjectiveTypes/SeasonsObjective.h"

namespace online
{
namespace liveops
{

class Challenge {
public:
	Challenge(ChallengeConfig, minecraft::api::ChallengeResponse);

	bool IsCompleted() const;
	const std::string& GetName() const;
	bool IsActive() const;

	std::vector<std::unique_ptr<Objective>> objectives;

	const minecraft::api::ChallengeResponse& GetData() const;
private:
	ChallengeConfig config;
	minecraft::api::ChallengeResponse data;
};



class Challenges {
public:
	Challenge* Add(ChallengeConfig, const minecraft::api::ChallengeResponse);

	std::vector<std::unique_ptr<Challenge>> challenges;
};

}
}
