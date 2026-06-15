#pragma once

#include "CoreMinimal.h"
#include "SeasonsObjective.h"

namespace online
{
namespace liveops
{

class ChestOpenedObjective : public Objective {
public:
	ChestOpenedObjective(ChallengeConfig, minecraft::api::ObjectiveResponse);
	virtual ~ChestOpenedObjective();

private:
	void OnChestOpened();
};

}
}
