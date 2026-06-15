#pragma once

#include "ObjectiveLocations.generated.h"

USTRUCT(BlueprintType)
struct FObjectiveLocations
{
	GENERATED_USTRUCT_BODY()

	FObjectiveLocations();
	FObjectiveLocations(TArray<FVector>);

	bool operator==(const FObjectiveLocations&) const;
	bool operator!=(const FObjectiveLocations&) const;

	UPROPERTY(BlueprintReadWrite)
	TArray<FVector> Locations;
	UPROPERTY(BlueprintReadWrite)
	bool bUseLocations;
};
