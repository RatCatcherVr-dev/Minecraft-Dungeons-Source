#pragma once
#include "game/item/InventoryItemData.h"
#include "game/item/ItemArchetypeCounts.h"
#include "game/mission/dlc/EligibleDLC.h"
#include "game/mission/offerings/MissionOfferings.h"
#include "game/mission/offerings/OfferingsSummary.h"
#include "game/mission/MissionSelection.h"
#include "game/mission/difficulty/MissionDifficulty.h"
#include "game/dlc/DLCName.h"
#include <UnrealString.h>

#include "MissionState.generated.h"

typedef uint32_t RandomSeed;

USTRUCT(BlueprintType)
struct DUNGEONS_API FMissionState {	
	GENERATED_BODY()

	UPROPERTY()
	FMissionDifficulty missionDifficulty;

	UPROPERTY(BlueprintReadOnly)
	int32 seed = 0;

	UPROPERTY(BlueprintReadOnly)
	TArray<FInventoryItemData> offeredItems;

	UPROPERTY(BlueprintReadOnly)
	TArray<EDLCName> ownedDLCs;
	
	UPROPERTY(BlueprintReadOnly)
	int offeredEnchantmentPoints = 0;

	UPROPERTY(BlueprintReadOnly)
	int livesLost = 0;

	UPROPERTY(BlueprintReadOnly)
	int partsDiscovered = 0;

	UPROPERTY()
	FString guid;

	UPROPERTY(BlueprintReadOnly)
	bool bCompletedOnce = false;

	FMissionState() = default;	
	FMissionState(const FMissionDifficulty&, RandomSeed seed, TArray<FInventoryItemData> items, int points, TArray<EDLCName>);
	FMissionState(const FMissionDifficulty&, RandomSeed seed, const FMissionOfferings&, TArray<EDLCName>);
	FMissionState(const FMissionDifficulty&, RandomSeed seed);

	int getItemCount() const;
	int getTotalEnchantmentPoints() const;
	int getTotalDisplayItemPower() const;	

	FEligibleDLC getEligibleDLCs() const;

	ItemArchetypeCounts getItemArchetypeCounts() const;

	EGameDifficulty getDifficulty() const;

	EThreatLevel getThreatLevel() const;

	FEndlessStruggle getEndlessStruggle() const;

	ELevelNames getLevelName() const;

	RandomSeed getSeed() const;

	FOfferingsSummary getOfferingsSummary() const;	
};
