#pragma once

#include "CoreMinimal.h"
#include "SeasonsObjective.h"

class UPlayerExperienceComponent;
class ULiveOps;
class UGameInstance;

namespace online {
namespace liveops {

class XPObjectiveType : public Objective {

public:
	XPObjectiveType(ChallengeConfig, minecraft::api::ObjectiveResponse);
	virtual ~XPObjectiveType();

private:
	void OnXPChanged(int32 XPChanged);
};
	
}
}
