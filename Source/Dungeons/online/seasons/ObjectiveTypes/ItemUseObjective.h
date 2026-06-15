#pragma once

#include "CoreMinimal.h"
#include "SeasonsObjective.h"
#include "GameplayEffectTypes.h"

class ItemType;

namespace online
{
namespace liveops
{

class ItemUseObjective : public Objective {
public:
	ItemUseObjective(ChallengeConfig, minecraft::api::ObjectiveResponse);
	virtual ~ItemUseObjective();

private:
	void OnItemUse(const ItemType& id);
};

}
}
