#include "ItemUseObjective.h"
#include "online/seasons/LiveOps.h"
#include "DungeonsGameInstance.h"
#include "online/sessions/OnlineUtil.h"
#include "online/seasons/Util/Filter.h"

namespace online
{
namespace liveops
{

ItemUseObjective::ItemUseObjective(ChallengeConfig config, minecraft::api::ObjectiveResponse objective)
	: Objective(config, objective) {
	CallbackHandle = online::getLiveOps(GameInstance)->OnItemUsed.AddRaw(this, &ItemUseObjective::OnItemUse);
}

ItemUseObjective::~ItemUseObjective() {
	online::getLiveOps(GameInstance)->OnItemUsed.Remove(CallbackHandle);
}

void ItemUseObjective::OnItemUse(const ItemType& itemType) {
	if (filter::itemuse::isMatch(filter, itemType)) {
		TriggerUpdate(1);
	}
}

}
}
