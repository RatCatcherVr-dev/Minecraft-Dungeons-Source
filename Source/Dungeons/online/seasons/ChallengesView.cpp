#include "ChallengesView.h"

#include "ChallengesHandler.h"
#include "ProgressHandler.h"
#include "RewardsHandler.h"
#include "Util/StringUtil.h"

namespace online
{
namespace liveops
{

ChallengesView ChallengesViewBuilder::Build(ChallengesHandler* challengesHandler, RewardsHandler* rewardsHandler, ProgressHandler* progressHandler)
{
	ChallengesView view;
	view.Challenges.reserve(5);

	for (const std::unique_ptr<Challenge>& challenge : challengesHandler->GetChallenges().challenges) {
		const unique_ptr<Objective>& objective = challenge->objectives.front();
		const auto& rewardId = challenge->GetData().rewards.front().rewardId;
		const auto& reward = rewardsHandler->GetRewardData(rewardId);

		if (reward.IsSet()) {
			view.Challenges.push_back({
				static_cast<int>(progressHandler->GetStatValue(objective->GetData().progressName).Get(0)),
				static_cast<int>(objective->GetData().requiredScore),
				stringutil::toFText(challenge->GetName()),  // TODO: Replace with description once available
				stringutil::toFString(challenge->GetName()),
				{
					FName(reward.GetValue().itemNameId.Get("").c_str()),
					reward.GetValue().type,
					reward.GetValue().state.Get(minecraft::api::RewardState::Locked)
				}
			});
			
		}
	}

	return view;
}

}
}
