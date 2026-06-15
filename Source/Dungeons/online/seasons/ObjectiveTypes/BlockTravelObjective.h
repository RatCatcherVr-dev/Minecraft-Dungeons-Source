#pragma once

#include "CoreMinimal.h"
#include "Optional.h"
#include "SeasonsObjective.h"
#include "world/level/BlockPos.h"

namespace online
{
namespace liveops
{

class BlockTravelObjective : public Objective {
	static constexpr int BLOCK_REPORT_BATCH_SIZE = 50;

public:
	BlockTravelObjective(ChallengeConfig, minecraft::api::ObjectiveResponse);
	virtual ~BlockTravelObjective();

private:
	void OnMovement(float deltaSeconds, const FVector& oldLocation, const FVector& oldVelocity);

	TOptional<BlockPos> previousBlock;
	int blockCount;
};

}
}
