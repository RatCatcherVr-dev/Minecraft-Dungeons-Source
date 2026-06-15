#pragma once

#include "CoreMinimal.h"
#include "SeasonsObjective.h"
#include "GameplayEffectTypes.h"

namespace online
{
namespace liveops 
{
class HealObjective : public Objective {
public:
	HealObjective(ChallengeConfig, minecraft::api::ObjectiveResponse);
	virtual ~HealObjective();

private:
	void OnHealing(float, const FOnAttributeChangeData&);
};
}
}
