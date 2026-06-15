#pragma once

#include "game/levels.h"
#include "game/affector/AffectorData.h"
#include "game/mission/MissionData.h"
#include "game/item/ItemRarity.h"
#include "game/difficulty/ExtraChallenge.h"
#include "game/util/ReplicatableMap.h"
#include "game/mission/theme/MissionTheme.h"
#include <Optional.h>
#include "TrialTypes.generated.h"

struct FTrialDef {
	ELevelNames level;
	EExtraChallenge extraChallenge;
	TArray<FMissionItemChance> rewards;	
	affector::RuleData affectors;
	TOptional<EMissionTheme> theme;
	FString type;

	FString id = FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphens);
};

/**
 * Simplified definition of a trial. Used for replication.
 */
USTRUCT()
struct DUNGEONS_API FReplicatableTrialDef {
	GENERATED_BODY()
	
	FTrialDef ToTrialDef() const;
	
	FString ToString() const;

	UPROPERTY()
	ELevelNames level;

	UPROPERTY()
	EExtraChallenge extraChallenge;

	UPROPERTY()
	FReplicatableMap modifiers;

	UPROPERTY()
	TArray<FReplicatableMap> rewards;

	UPROPERTY()
	FString id;

	UPROPERTY()
	FString type;

	UPROPERTY()
	EMissionTheme Theme = EMissionTheme::Invalid;

private:
	affector::RuleData ParseAffectors() const;
	
	TArray<FMissionItemChance> ParseRewards() const;

	static TOptional<FMissionItemChance> ParseTagChanceReward(const FReplicatableMap& reward);
	static TOptional<FMissionItemChance> ParseTypeChanceReward(const FReplicatableMap& reward);
	static FMissionItemChance ParseRarityChanceReward(const FReplicatableMap& reward);

	static EItemRarity ParseRarity(const FReplicatableMap& reward);
	static float ParseWeight(const FReplicatableMap& reward);
};

USTRUCT()
struct DUNGEONS_API FReplicatableTrialDefPack {
	GENERATED_BODY()

	void Set(const TArray<FReplicatableTrialDef>&);

	bool IsSet() const;
	void MarkSet();

	TArray<FTrialDef> ToTrialDefs() const;

private:
	UPROPERTY()
	TArray<FReplicatableTrialDef> trialDefs;

	UPROPERTY()
	bool isSet = false;
};
