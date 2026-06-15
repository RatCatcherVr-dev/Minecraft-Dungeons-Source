#pragma once

#include "MissionDifficulty.h"
#include <Kismet/BlueprintFunctionLibrary.h>
#include "MissionDifficultyUtil.generated.h"

UCLASS(BlueprintType)
class UMissionDifficultyUtil : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static bool IsMissionDifficultyValid(const FMissionDifficulty& missionDifficulty);
};
