#pragma once
#include <memory>

#include "LiveIF.h"
#include "Engine/EngineTypes.h"
#include "modules/LiveOps/ChallengeResponse.h"
#include "modules/LiveOps/ProgressResponse.h"

class UGameInstance;
class AActor;

namespace minecraft {
	namespace api {
		struct Reward;
	}
}

namespace online
{
namespace liveops
{
	struct ClaimConfig;


	class RewardType {
public:
	static std::unique_ptr<RewardType> MakeReward(const minecraft::api::Reward&, UGameInstance*);
	RewardType(const minecraft::api::Reward&, UGameInstance*);
	virtual ~RewardType() {}

	virtual void Claim(const ClaimConfig&);
	void SetClaimed();
	const minecraft::api::Reward& GetData() const;

protected:
	virtual void ClaimInternal(const ClaimConfig&) = 0;
	minecraft::api::Reward reward;
	UGameInstance* GameInstance;
};

class EmeraldsReward : public RewardType {
public:
	EmeraldsReward(const minecraft::api::Reward&, UGameInstance*);
private:
	void ClaimInternal(const ClaimConfig&) override;
};

class GoldReward : public RewardType {
public:
	GoldReward(const minecraft::api::Reward&, UGameInstance*);
private:
	void ClaimInternal(const ClaimConfig&) override;
};

class ItemReward : public RewardType {
public:
	ItemReward(const minecraft::api::Reward&, UGameInstance*);
private:
	void ClaimInternal(const ClaimConfig&) override;
};

class CosmeticReward : public RewardType {
public:
	CosmeticReward(const minecraft::api::Reward&, UGameInstance*);
	~CosmeticReward();
private:
	void ClaimInternal(const ClaimConfig&) override;
	void UnbindHandles();
	FDelegateHandle EntitlementsProvided;
	FDelegateHandle FailedRequest;
	int Retries = 0;
	FTimerHandle TimerHandle;
};

class AdventurePointsReward : public RewardType {
public:
	AdventurePointsReward(const minecraft::api::Reward&, UGameInstance*);
private:
	void ClaimInternal(const ClaimConfig&) override;
};
	
}
}
