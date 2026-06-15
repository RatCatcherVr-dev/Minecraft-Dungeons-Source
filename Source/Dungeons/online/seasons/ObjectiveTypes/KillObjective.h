#pragma once

#include "CoreMinimal.h"
#include "SeasonsObjective.h"
#include "world/entity/EntityTypes.h"

namespace online {
namespace liveops {


class KillObjective : public Objective {
public:
	KillObjective(ChallengeConfig, minecraft::api::ObjectiveResponse);
	virtual ~KillObjective();

private:
	void OnMobDeath(EntityType mobType);
};
}
}
