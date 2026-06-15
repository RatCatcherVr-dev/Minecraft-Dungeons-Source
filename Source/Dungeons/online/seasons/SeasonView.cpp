#include "SeasonView.h"

#include "ProgressHandler.h"
#include "RewardsHandler.h"
#include "modules/LiveOps/SeasonResponse.h"
#include "Util/SeasonsCommon.h"
#include "Util/StringUtil.h"
#include "SeasonsHandler.h"

namespace online
{
namespace liveops
{

namespace internal
{

void UpdateSeasonMetadata(SeasonView& view, const Season& season)
{
	view.Number = season.GetNumber();
	view.Name = stringutil::toFText(season.GetDisplayName());
	view.StartDateTime = season.GetStartDateTimeUTC();
	view.EndDateTime = FDateTime(1970, 1, 1);
}

void UpdatePlayerProgress(SeasonView& view, const Season& season, ProgressHandler* progressHandler)
{
	view.PlayerLevel = 0;
	view.PlayerLevelProgress = 0.0f;

	const std::vector<minecraft::api::SeasonRewardLevel>& rewardLevels = season.GetRewardLevels();
	const int64 playerAP = progressHandler->GetStatValue(season.GetAPProgressName()).Get(0);

	for (size_t i = 0; i < rewardLevels.size(); i++) {
		if (rewardLevels[i].apRequired > playerAP) {
			const int64 nextLevelAP = rewardLevels[i].apRequired;
			const int64 currentLevelAP = i > 0 ? rewardLevels[i - 1].apRequired : 0;
			view.PlayerLevelProgress = static_cast<float>(playerAP - currentLevelAP) / (nextLevelAP - currentLevelAP);
			break;
		}

		view.PlayerLevel++;
	}
}

SeasonRewardView BuildSeasonRewardView(const minecraft::api::Reward& rewardInfo)
{
	SeasonRewardView rewardView;
	rewardView.ItemNameId = stringutil::toFName(rewardInfo.itemNameId.Get(""));
	rewardView.Type = rewardInfo.type;
	rewardView.State = rewardInfo.state.Get(minecraft::api::RewardState::Locked);
	rewardView.RewardId = rewardInfo.itemNameId.Get("");
	return rewardView;
}

void UpdateRewardState(SeasonView& view, const Season& season, ProgressHandler* progressHandler, RewardsHandler* rewardsHandler)
{
	auto seasonRewardViewBuilder = [&] (std::vector<minecraft::api::RewardId> rewardIds) -> TOptional<SeasonRewardView> {
		if (!rewardIds.empty()) {
			const auto& reward = rewardsHandler->GetRewardData(rewardIds.front().rewardId);
			if (reward.IsSet()) {
				return internal::BuildSeasonRewardView(reward.GetValue());
			}
		}
		return {};
	};

	for (const minecraft::api::SeasonRewardLevel& rewardLevel : season.GetRewardLevels()) {
		SeasonLevelView levelView;

		levelView.RequiredAP = rewardLevel.apRequired;
		levelView.FreeReward = seasonRewardViewBuilder(rewardLevel.rewards.free);
		levelView.PaidReward = seasonRewardViewBuilder(rewardLevel.rewards.paid);

		view.RewardLevels.push_back(levelView);
	}
}

}

SeasonView SeasonViewBuilder::Build(SeasonsHandler* seasonsHandler, RewardsHandler* rewardsHandler, ProgressHandler* progressHandler)
{
	SeasonView view = {
		1,
		stringutil::toFText("N/A"),
		FDateTime(1970, 1, 1),
		FDateTime(1970, 1, 1),
		0,
		0.0f,
		{}
	};

	const auto& seasons = seasonsHandler->GetSeasons();

	if (!seasons.empty()) {
		const auto& season = seasons.front();
		internal::UpdateSeasonMetadata(view, season);
		internal::UpdatePlayerProgress(view, season, progressHandler);
		internal::UpdateRewardState(view, season, progressHandler, rewardsHandler);
	}

	return view;
}

}
}
