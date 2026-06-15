#include "KillObjective.h"
#include "online/sessions/OnlineUtil.h"
#include "online/seasons/LiveOps.h"
#include "DungeonsGameInstance.h"


namespace online {
namespace liveops {
	

KillObjective::KillObjective(ChallengeConfig config, minecraft::api::ObjectiveResponse objective)
	: Objective(config, objective) {
	CallbackHandle = online::getLiveOps(GameInstance)->OnKilledOther.AddRaw(this, &KillObjective::OnMobDeath);
}

KillObjective::~KillObjective() {
	online::getLiveOps(GameInstance)->OnKilledOther.Remove(CallbackHandle);
}

void KillObjective::OnMobDeath(EntityType mobType) {
	if (online::liveops::filter::killobjective::isMatch(filter, mobType)) {
		TriggerUpdate(1);
	}
}

}
}
