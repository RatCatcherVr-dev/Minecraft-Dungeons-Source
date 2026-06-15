#include "HealObjective.h"
#include "online/seasons/LiveOps.h"
#include "DungeonsGameInstance.h"
#include "online/sessions/OnlineUtil.h"
#include "online/seasons/Util/Filter.h"

namespace online
{
namespace liveops
{

HealObjective::HealObjective(ChallengeConfig config, minecraft::api::ObjectiveResponse objective)
	: Objective(config, objective) {
	CallbackHandle = online::getLiveOps(GameInstance)->OnHealed.AddRaw(this, &HealObjective::OnHealing);
}
HealObjective::~HealObjective() {
	online::getLiveOps(GameInstance)->OnHealed.Remove(CallbackHandle);
}

void HealObjective::OnHealing(float amount, const FOnAttributeChangeData& attributeData) {
	if (filter::healingdone::isMatch(filter, attributeData)) {
		TriggerUpdate(1);
	}
}
}
}
