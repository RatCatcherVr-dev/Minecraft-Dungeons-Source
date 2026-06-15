#pragma once

#include "EmergentDifficulty.generated.h"

USTRUCT(BlueprintType)
struct DUNGEONS_API FEmergentDifficulty
{
	GENERATED_USTRUCT_BODY()

	FEmergentDifficulty()
	{

	}

	FEmergentDifficulty(int32 startRaidDifficulty, int32 midGameAffetorsNum)
		:raidDifficulty(startRaidDifficulty)
		, midGameAffectorsNum(midGameAffetorsNum)
	{
	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 raidDifficulty = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 midGameAffectorsNum = 0;

	static constexpr float perGlobalThreatFraction = 1.f;

	bool IsAtMaxRaidDifficulty() const
	{
		return raidDifficulty >= 3;
	}

	bool IsAtMaxMidGameAffectors() const
	{
		return midGameAffectorsNum >= 3;
	}

	int32 GetTotalTokensCollected() const {
		return raidDifficulty + midGameAffectorsNum;
	}

	FEmergentDifficulty GetNextRaidDifficulty() const {
		return FEmergentDifficulty(raidDifficulty + 1, midGameAffectorsNum);
	}

	FEmergentDifficulty GetWithChangedAffectorsNum(int32 newMidGameAffectors) const {
		return FEmergentDifficulty(raidDifficulty, newMidGameAffectors);
	}
};
