#include "ChestOpenedObjective.h"
#include "online/seasons/LiveOps.h"
#include "DungeonsGameInstance.h"
#include "online/sessions/OnlineUtil.h"

namespace online
{
namespace liveops
{

ChestOpenedObjective::ChestOpenedObjective(ChallengeConfig config, minecraft::api::ObjectiveResponse objective)
	: Objective(config, objective) {
	CallbackHandle = online::getLiveOps(GameInstance)->OnOpenChest.AddRaw(this, &ChestOpenedObjective::OnChestOpened);
}

ChestOpenedObjective::~ChestOpenedObjective() {
	online::getLiveOps(GameInstance)->OnOpenChest.Remove(CallbackHandle);
}

void ChestOpenedObjective::OnChestOpened() {
	TriggerUpdate(1);
}

}
}
