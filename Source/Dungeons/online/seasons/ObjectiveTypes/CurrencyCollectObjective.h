#pragma once

#include "CoreMinimal.h"
#include "SeasonsObjective.h"

struct FSerializableItemId;
class UGameInstance;

namespace online {
namespace liveops {

class CurrencyCollectObjective : public Objective {
public:
	CurrencyCollectObjective(ChallengeConfig, minecraft::api::ObjectiveResponse);
	virtual ~CurrencyCollectObjective();

private:
	void OnCurrencyChanged(const FName& type, int32 amount);
};
}
}
