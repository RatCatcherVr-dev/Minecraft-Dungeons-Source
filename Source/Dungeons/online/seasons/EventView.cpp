#include "EventView.h"

#include "ProgressHandler.h"
#include "RewardsHandler.h"
#include "Util/StringUtil.h"
#include "SeasonsHandler.h"

namespace online
{
namespace liveops
{

namespace internal
{

void UpdateEventMetadata(EventView& view, const Event& event)
{
	view.Name = FText::AsCultureInvariant(stringutil::toFString(event.GetDisplayName()));
	view.StartDateTime = event.GetStartDateTimeUTC();
	view.EndDateTime = event.GetEndDateTimeUTC();
}

EventRewardView BuildEventRewardView(const minecraft::api::Reward& rewardInfo)
{
	EventRewardView rewardView;
	rewardView.ItemNameId = stringutil::toFName(rewardInfo.itemNameId.Get(""));
	rewardView.Type = rewardInfo.type;
	rewardView.State = rewardInfo.state.Get(minecraft::api::RewardState::Locked);
	rewardView.RewardId = rewardInfo.rewardId;
	return rewardView;
}

void UpdateRewardState(EventView& view, const Event& event, ProgressHandler* progressHandler, RewardsHandler* rewardsHandler)
{
	auto eventRewardViewBuilder = [&](std::vector<minecraft::api::RewardId> rewardIds) -> TOptional<EventRewardView> {
		if (!rewardIds.empty()) {
			const auto& reward = rewardsHandler->GetRewardData(rewardIds.front().rewardId);
			if (reward.IsSet()) {
				return internal::BuildEventRewardView(reward.GetValue());
			}
		}
		return {};
	};

	for (const minecraft::api::SeasonRewardLevel& rewardLevel : event.GetRewardLevels()) {
		EventLevelView levelView;

		levelView.RequiredAP = rewardLevel.apRequired;
		levelView.FreeReward = eventRewardViewBuilder(rewardLevel.rewards.free);
		levelView.PaidReward = eventRewardViewBuilder(rewardLevel.rewards.paid);

		view.RewardLevels.push_back(levelView);
	}
}

void UpdateChallengesView(EventView& view, ChallengesHandler* challengesHandler, RewardsHandler* rewardsHandler, ProgressHandler* progressHandler) {
	view.ChallengeView = ChallengesViewBuilder::Build(challengesHandler, rewardsHandler, progressHandler);
}

void ModifyChallengeRewardToEventReward(EventView& view) {
	auto modifyReward = [](ChallengeRewardView& challengeReward, const EventRewardView& eventReward) {
			challengeReward.State = eventReward.State;
			challengeReward.Type = eventReward.Type;
			challengeReward.ItemNameId = eventReward.ItemNameId;
	};

	for (int i = 0; i < view.ChallengeView.Challenges.size(); i++) {
		auto& challengeReward = view.ChallengeView.Challenges.at(i).Reward;
		if (view.RewardLevels.size() > i) {
			modifyReward(challengeReward, view.RewardLevels.at(i).FreeReward.GetValue());
		} else if (!view.RewardLevels.empty()) {
			modifyReward(challengeReward, view.RewardLevels.front().FreeReward.GetValue());
		}
	}
}
}

TOptional<EventView> EventViewBuilder::Build(SeasonsHandler* seasonsHandler, RewardsHandler* rewardsHandler, ProgressHandler* progressHandler, ChallengesHandler* challengesHandler)
{
	TOptional<EventView> eventView;
	TOptional<Event> currentEvent = seasonsHandler->GetCurrentEvent();

	if (currentEvent.IsSet()) {
		eventView.Emplace();
		internal::UpdateEventMetadata(eventView.GetValue(), currentEvent.GetValue());
		internal::UpdateRewardState(eventView.GetValue(), currentEvent.GetValue(), progressHandler, rewardsHandler);
		internal::UpdateChallengesView(eventView.GetValue(), challengesHandler, rewardsHandler, progressHandler);
		internal::ModifyChallengeRewardToEventReward(eventView.GetValue());
	}

	return eventView;
}

}
}
