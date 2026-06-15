#include "CurrencyCollectObjective.h"

#include "online/sessions/OnlineUtil.h"
#include "online/seasons/LiveOps.h"
#include "DungeonsGameInstance.h"
#include "online/seasons/Util/Filter.h"


namespace online {
namespace liveops {

CurrencyCollectObjective::CurrencyCollectObjective(ChallengeConfig config, minecraft::api::ObjectiveResponse objective)
	: Objective(config, objective) {
	CallbackHandle = online::getLiveOps(GameInstance)->OnCurrencyChanged.AddRaw(this, &CurrencyCollectObjective::OnCurrencyChanged);
}

CurrencyCollectObjective::~CurrencyCollectObjective() {
	online::getLiveOps(GameInstance)->OnCurrencyChanged.Remove(CallbackHandle);
}

void CurrencyCollectObjective::OnCurrencyChanged(const FName& typeName, int32 amount) {
	if (filter::currencycollected::isMatch(filter, GameInstance, typeName, amount)) {
		TriggerUpdate(amount);
	}
}
}
}
