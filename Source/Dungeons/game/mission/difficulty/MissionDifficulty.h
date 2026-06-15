#pragma once

#include "game/levels.h"
#include "game/difficulty/Difficulty.h"
#include "game/difficulty/ThreatLevel.h"
#include "game/difficulty/EndlessStruggle.h"
#include "game/difficulty/ExtraChallenge.h"
#include "MissionDifficulty.generated.h"

struct FMissionProgress;
struct FMissionSelection;

USTRUCT(BlueprintType)
struct FMissionDifficulty {
	GENERATED_BODY()

	int compare(const FMissionDifficulty& rhs) const;	
	bool operator< (const FMissionDifficulty& rhs) const { return compare(rhs) < 0; }
	bool operator>= (const FMissionDifficulty& rhs) const { return compare(rhs) >= 0; }
	bool operator==(const FMissionDifficulty& rhs) const { return compare(rhs) == 0; }
	bool operator!=(const FMissionDifficulty& rhs) const { return compare(rhs) != 0; }
	
	static FMissionDifficulty from(const FMissionProgress& progress);
	static FMissionDifficulty from(const FMissionSelection& selection);
	operator game::FDifficulty() const;

	UPROPERTY(BlueprintReadWrite)
	ELevelNames mission;
	UPROPERTY(BlueprintReadWrite)
	EGameDifficulty difficulty;
	UPROPERTY(BlueprintReadWrite)
	EThreatLevel threatLevel;
	UPROPERTY(BlueprintReadWrite)
	FEndlessStruggle endlessStruggle;

	bool IsValid() const;
};
