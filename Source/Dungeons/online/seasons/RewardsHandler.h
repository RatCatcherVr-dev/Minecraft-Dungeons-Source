#pragma once

#include <string>
#include <memory>
#include <set>


#include "LiveIF.h"
#include "Rewards.h"
#include "core/HttpServiceResponse.h"
#include "modules/LiveOps/SeasonResponse.h"

namespace online
{
namespace liveops
{

using RewardKey = std::string;
using RewardRepo = std::map<RewardKey, std::unique_ptr<RewardType>>;

enum class ClaimResult {
	Succeeded,
	Failed,
	NotClaimable,
	InternalError,
	Unknown
};

struct ClaimConfig {
	std::vector<RewardKey> keys;
	std::string seasonName;
	AActor* player;
	std::function<void(ClaimResult)> callback = [] (ClaimResult) {};
};

class RewardsHandler : public LiveIF {
public:
	RewardsHandler(std::shared_ptr<LiveOpsClient> apiClient, UGameInstance* gameInstance);

	virtual void Init() override;
	virtual void Teardown() override;
	virtual void Request() override;
	virtual void MultiRequestPartial(std::shared_ptr<MultiRequest> multiRequest) override;

	void Claim(ClaimConfig) const;
	TOptional<minecraft::api::Reward> GetRewardData(RewardKey) const;

private:
	bool IsAllUnlocked(ClaimConfig config) const;
	void InternalUpdateRewards(const std::vector<minecraft::api::Reward>&);
	std::vector<RewardKey> GetAutoClaimableRewards(minecraft::api::HttpServiceResponse<minecraft::api::RewardResponse>) const;
	bool AutoClaim(minecraft::api::HttpServiceResponse<minecraft::api::RewardResponse> response,
	               std::shared_ptr<MultiRequest> multiRequest, std::vector<RewardKey> claimRewards) const;
	bool TryAutoClaim(minecraft::api::HttpServiceResponse<minecraft::api::RewardResponse>, std::shared_ptr<MultiRequest>) const;
	RewardType* GetReward(RewardKey) const;
	void ClaimInternal(const ClaimConfig&) const;
	void RegisterReward(const minecraft::api::Reward&);
	RewardRepo rewardRepo;
};

}
}
