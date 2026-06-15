#include "BlockTravelObjective.h"
#include "online/seasons/LiveOps.h"
#include "DungeonsGameInstance.h"
#include "game/Conversion.h"
#include "online/sessions/OnlineUtil.h"

namespace online
{
namespace liveops
{

BlockTravelObjective::BlockTravelObjective(ChallengeConfig config, minecraft::api::ObjectiveResponse objective)
	: Objective(std::move(config), std::move(objective))
	, blockCount(0)
{
	CallbackHandle = online::getLiveOps(GameInstance)->OnMovement.AddRaw(this, &BlockTravelObjective::OnMovement);
}

BlockTravelObjective::~BlockTravelObjective()
{
	online::getLiveOps(GameInstance)->OnMovement.Remove(CallbackHandle);
}

void BlockTravelObjective::OnMovement(float deltaSeconds, const FVector& oldLocation, const FVector& oldVelocity)
{
	const FVector newLocation = oldLocation + oldVelocity * deltaSeconds;
	const BlockPos newBlock = conversion::ueToBlock(newLocation);

	if (!previousBlock.IsSet()) {
		previousBlock = newBlock;
	}

	if (newBlock != previousBlock) {
		previousBlock = newBlock;
		++blockCount;
	}

	if (blockCount >= BLOCK_REPORT_BATCH_SIZE) {
		TriggerUpdate(blockCount);
		blockCount = 0;
	}
}

}
}
