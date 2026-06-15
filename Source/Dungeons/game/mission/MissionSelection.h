#pragma once
#include "game/levels.h"
#include "game/difficulty/Difficulty.h"
#include "game/difficulty/ThreatLevel.h"
#include "MissionSelection.generated.h"

USTRUCT(BlueprintType)
struct DUNGEONS_API FMissionSelection {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	ELevelNames levelName = ELevelNames::Invalid;

	UPROPERTY(BlueprintReadWrite)
	EGameDifficulty difficulty = EGameDifficulty::Invalid;

	UPROPERTY(BlueprintReadWrite)
	EThreatLevel threatLevel = EThreatLevel::Invalid;

	UPROPERTY(BlueprintReadWrite)
	FEndlessStruggle endlessStruggle;
};
