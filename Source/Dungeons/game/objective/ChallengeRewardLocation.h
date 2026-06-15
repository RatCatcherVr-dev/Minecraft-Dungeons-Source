#pragma once

#include "CoreMinimal.h"
#include "ChallengeRewardLocation.generated.h"

USTRUCT(BlueprintType)
struct FChallengeRewardLocation
{
	GENERATED_BODY()

	UPROPERTY()
	int ChallengeID;

	UPROPERTY()
	FVector Location;

	friend bool operator==(const FChallengeRewardLocation& Lhs, const FChallengeRewardLocation& Rhs)
	{
		return Lhs.ChallengeID == Rhs.ChallengeID;

	}
};