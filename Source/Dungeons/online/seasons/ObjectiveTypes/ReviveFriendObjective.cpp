#include "ReviveFriendObjective.h"
#include "online/sessions/OnlineUtil.h"
#include "DungeonsGameInstance.h"
#include "online/seasons/LiveOps.h"
#include "online/seasons/Util/Filter.h"

namespace online
{
namespace liveops
{
	
ReviveFriendObjective::ReviveFriendObjective(ChallengeConfig config, minecraft::api::ObjectiveResponse objective)
	: Objective(config, objective) {
	CallbackHandle = online::getLiveOps(GameInstance)->OnRevive.AddRaw(this, &ReviveFriendObjective::OnReviveFriend);
}

ReviveFriendObjective::~ReviveFriendObjective() {
	online::getLiveOps(GameInstance)->OnRevive.Remove(CallbackHandle);
}

void ReviveFriendObjective::OnReviveFriend() {
	if (filter::revivefriend::isMatch(filter)) {
		TriggerUpdate(1);
	}
}

}
}
