#pragma once

#include "CoreMinimal.h"
#include "SeasonsObjective.h"

class UGameInstance;

namespace online 
{
namespace liveops 
{
class ReviveFriendObjective : public Objective {
public:
	ReviveFriendObjective(ChallengeConfig, minecraft::api::ObjectiveResponse);
	virtual ~ReviveFriendObjective();

private:
	void OnReviveFriend();
};
}
}

