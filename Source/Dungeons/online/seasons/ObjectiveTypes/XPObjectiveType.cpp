#include "XPObjectiveType.h"
#include "online/sessions/OnlineUtil.h"
#include "online/seasons/LiveOps.h"

namespace online
{
namespace liveops
{

XPObjectiveType::XPObjectiveType(ChallengeConfig config, minecraft::api::ObjectiveResponse objective)
	: Objective(config, objective) {
	CallbackHandle = online::getLiveOps(GameInstance)->OnXPChanged.AddRaw(this, &XPObjectiveType::OnXPChanged);
}

XPObjectiveType::~XPObjectiveType() {
	online::getLiveOps(GameInstance)->OnXPChanged.Remove(CallbackHandle);
}

void XPObjectiveType::OnXPChanged(int32 XPChanged) {
	TriggerUpdate(XPChanged);
}

}
}
