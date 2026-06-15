#pragma once

#include "CoreMinimal.h"
#include "modules/LiveOps/ChallengeResponse.h"
#include "online/seasons/Rewards.h"

#include <memory>


class UGameInstance;

namespace online
{
namespace liveops
{

struct FilterPart {
	minecraft::api::FilterPartType type;
	std::string value;
};


using IsChallengeActive = std::function<bool()>;

struct ChallengeConfig {
	UGameInstance* GameInstance;
	std::shared_ptr<LiveOpsClient> apiClient;
	IsChallengeActive isActive;
};

class Objective {
public:
	Objective(ChallengeConfig, minecraft::api::ObjectiveResponse);
	virtual ~Objective() = default;

	minecraft::api::ObjectiveType GetType() const;
	const std::string& GetProgressName() const;
	const minecraft::api::ObjectiveResponse& GetData() const;

	std::vector<FilterPart> filter;

protected:
	void TriggerUpdate(int64 amount) const;

	FDelegateHandle CallbackHandle;
	UGameInstance* GameInstance;

	IsChallengeActive isActive;

private:
	minecraft::api::ObjectiveResponse data;
};

std::unique_ptr<Objective> MakeObjective(ChallengeConfig, const minecraft::api::ObjectiveResponse&);

}
}
