#pragma once
#include "CoreMinimal.h"

#include "LiveIF.h"
#include "modules/LiveOps/ChallengeResponse.h"
#include "Util/Filter.h"
#include <set>

namespace online
{
namespace liveops
{


class AdventurePointsHandler : public LiveIF {
public:
	using TriggerUpdate = std::function<void(int64)>;
	AdventurePointsHandler(std::shared_ptr<LiveOpsClient>, UGameInstance*);
	virtual void Init() override;
	virtual void Teardown() override;
	virtual void Request() override;
	virtual void MultiRequestPartial(std::shared_ptr<MultiRequest> multiRequest) override;
	void SetConfig(const minecraft::api::AdventurePointsConfig&);

public:
	struct FilterConfig {
		TriggerUpdate triggerUpdate;
		std::vector<FilterPart> filter;
		int64 scoreGranted;
		int64 perCompleted;
		int64 currentScore;
	};
	using Handles = std::vector<FDelegateHandle>;

private:
	TriggerUpdate GetTriggerUpdateFunction() const;
	void SetupConfig();
	void ClearHandles();

	minecraft::api::AdventurePointsConfig data;
	std::vector<FDelegateHandle> handles;

	Handles MissionCompletedHandles;
	Handles MobDeathHandles;
	Handles XpGainedHandles;
	Handles PlayerHealedHandles;
	Handles ReviveFriendHandles;
	Handles CurrencyCollectedHandles;
	Handles TrialCompletedHandles;
	Handles ChestOpenedHandles;
	Handles MovementHandles;
};
}
}
