#include "RewardsHandler.h"

#include "core/HttpServiceResponse.h"
#include "IHttpResponse.h"
#include "LeaderboardsHandler.h"
#include "LiveOpsClient.h"
#include "SeasonsHandler.h"
#include "util/SeasonsCommon.h"

namespace online
{
namespace liveops
{

namespace internal
{
minecraft::api::RewardClaimRequest MakeClaimRequest(const ClaimConfig& config) {
	auto rewards = std::make_shared<std::vector<minecraft::api::RewardClaim>>();
	auto time = minecraft::api::Datetime();
	for (auto key : config.keys) {
		rewards->push_back({
			std::make_shared<std::string>(key),
			std::make_shared<std::string>(config.seasonName),
			make_shared<std::string>(common::GenerateUUID(time.timeMillis.time_since_epoch().count()))
			});
	}

	return { rewards };
}

bool Unlocked(const minecraft::api::Reward& reward) {
	return reward.state.Get(minecraft::api::RewardState::Unavailable) == minecraft::api::RewardState::Unlocked;
}

bool IsAutoClaimable(const minecraft::api::Reward& reward) {
	return reward.type == minecraft::api::RewardTypes::AdventurePoints
		&& reward.state.Get(minecraft::api::RewardState::Unavailable) == minecraft::api::RewardState::Unlocked;
}

}

RewardsHandler::RewardsHandler(std::shared_ptr<LiveOpsClient> apiClient, UGameInstance* gameInstance)
	: LiveIF(apiClient, gameInstance) {
}

void RewardsHandler::Init() {
}

void RewardsHandler::Teardown() {
}

void RewardsHandler::Request() {
	MultiRequestPartial(std::shared_ptr<MultiRequest>());
}

void RewardsHandler::MultiRequestPartial(std::shared_ptr<MultiRequest> multiRequest) {
	LiveOpsClient::Callback<minecraft::api::RewardResponse> callback = [this, multiRequest](minecraft::api::HttpServiceResponse<minecraft::api::RewardResponse> response) {
		bool autoClaiming = false;
		if (response.successful()) {
			InternalUpdateRewards(response.getBody()->rewards);
			autoClaiming = TryAutoClaim(response, multiRequest);
		} else {
			UE_LOG(LogLiveOps, Warning, TEXT("[LiveOps] Failed to request rewards: %d"), response.getStatus());
		}

		if (!autoClaiming && multiRequest) {
			multiRequest->CompleteSingleRequest(response);
		}
	};
	mApiClient->Request<DungeonsEndpoint::REWARDS_GET>(callback);
}

bool RewardsHandler::IsAllUnlocked(ClaimConfig config) const {
	for (auto key : config.keys) {
		if (auto rewardData = GetRewardData(key)) {
			if (!internal::Unlocked(rewardData.GetValue())) {
				return false;
			}
		} else {
			config.callback(ClaimResult::InternalError);
		}
	}
	return true;
}

void RewardsHandler::Claim(ClaimConfig config) const {
	if (IsAllUnlocked(config)) {
		ClaimInternal(config);
	} else {
		config.callback(ClaimResult::NotClaimable);
	}
}

TOptional<minecraft::api::Reward> RewardsHandler::GetRewardData(RewardKey key) const {
	if (auto reward = GetReward(key)) {
		return reward->GetData();
	}
	return {};
}

void RewardsHandler::InternalUpdateRewards(const std::vector<minecraft::api::Reward>& rewards) {
	rewardRepo.clear();
	for (const auto& reward : rewards) {
		RegisterReward(reward);
	}
}

std::vector<RewardKey> RewardsHandler::GetAutoClaimableRewards(minecraft::api::HttpServiceResponse<minecraft::api::RewardResponse> response) const {
	std::vector<RewardKey> output;
	for (auto reward : response.getBody()->rewards) {
		if (internal::IsAutoClaimable(reward)) {
			output.push_back(reward.rewardId);
		}
	}
	return output;
}

bool RewardsHandler::AutoClaim(minecraft::api::HttpServiceResponse<minecraft::api::RewardResponse> response, std::shared_ptr<MultiRequest> multiRequest, std::vector<RewardKey> rewards) const {
	if (rewards.empty()) {
		return false;
	}
	Claim({
		rewards,
		online::getSelectedSeason(GameInstance),
		nullptr,
		[=](ClaimResult) mutable {
			if (multiRequest) {
				multiRequest->CompleteSingleRequest(response);
			}
		}
	});
	return true;
}

bool RewardsHandler::TryAutoClaim(minecraft::api::HttpServiceResponse<minecraft::api::RewardResponse> response, std::shared_ptr<MultiRequest> multiRequest) const {
	return AutoClaim(response, multiRequest, GetAutoClaimableRewards(response));
}

void RewardsHandler::RegisterReward(const minecraft::api::Reward& reward) {
	rewardRepo[reward.rewardId] = RewardType::MakeReward(reward, GameInstance);
}

RewardType* RewardsHandler::GetReward(RewardKey key) const {
	auto it = rewardRepo.find(key);
	if (it != rewardRepo.end()) {
		return it->second.get();
	}
	return nullptr;
}

void RewardsHandler::ClaimInternal(const ClaimConfig& config) const {
	LiveOpsClient::Callback<minecraft::api::RewardClaimResponse> callback = [&, config](minecraft::api::HttpServiceResponse<minecraft::api::RewardClaimResponse> response) {
		if (response.successful()) {
			for (auto key : config.keys) {
				if (auto reward = GetReward(key)) {
					if (internal::Unlocked(reward->GetData())) {
						reward->Claim(config);
					} else {
						config.callback(ClaimResult::NotClaimable);
					}
				} else {
					config.callback(ClaimResult::InternalError);
				}
			}
		} else {
			config.callback(ClaimResult::Failed);
			UE_LOG(LogLiveOps, Warning, TEXT("[LiveOps] Failed to claim RewardId"));
		}
	};
	mApiClient->Request<DungeonsEndpoint::REWARDS_POST>(callback, internal::MakeClaimRequest(config));
}
}
}
