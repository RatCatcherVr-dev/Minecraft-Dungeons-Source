// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "game/LevelSettings.h"
#include "util/DungeonsAwardGenerator.h"
#include <Containers/Map.h>
#include <random>
#include "game/actor/character/player/BasePlayerState.h"
#include "AwardsGeneratorComponent.generated.h"

class APlayerCharacter;
enum class EGameTrackingTypes : uint8;

USTRUCT(BlueprintType)
struct DUNGEONS_API FGameStatsStruct {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	FText Name;
	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	FString Id;

	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	int Count;
	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	int Total;
	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	bool HasPercentage;
	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	FText Percentage;

};

USTRUCT(BlueprintType)
struct DUNGEONS_API FAwardStruct {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	FText PlayerLocalDisplayText;
	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	FString PlayerPrimaryNameText;
	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	FString PlayerSecondaryNameText;
	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	int32 PlayerNumber;
	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	int32 PlayerXp;
	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	FColor PlayerColor;
	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	FName PlayerSkinId;
	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	int Score;
	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	EAwardType Type;
	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	bool IsMe;
	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	EPlatformType PlayerPlatform;
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FMissionFinishedSummary {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	FLevelSettings levelSettings;

	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	TArray<FAwardStruct> awards;

	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	TArray<FGameStatsStruct> gameStats;

	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	TArray<FReplicatableModifier> levelModifiers;

	UPROPERTY()
	FString trialIdOrBlank;

	TOptional<FString> GetTrialId() const;

	FMissionFinishedSummary GetCopyAdaptedToLocalControllers(UWorld* world) const;
};

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UAwardsGeneratorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAwardsGeneratorComponent();

	/** The default weight given to a type of award */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	int32 DefaultWeight = 1;

	/** The weight when sampling awards. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	TMap<EAwardType, int32> AwardWeights;

	/** The threshold a given award has to reach score wise to be considered valid. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	TMap<EAwardType, float> AwardThresholds;
	
	/** The weight when choosing game round award. */ //// todo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	TMap<EGameTrackingTypes, int32> StatsWeights;

	UPROPERTY(EditDefaultsOnly, BluePrintReadOnly, Category = "Dungeons")
	int32 NumberGameStatsDisplayed = 4;

	TArray<FAwardStruct> GenerateAwards(ELevelNames level = ELevelNames::creeperwoods, EGameDifficulty difficulty = EGameDifficulty::Difficulty_1) const;
	TArray<FGameStatsStruct> ProcessGameStats() const;
private:
	const APlayerCharacter* GetPlayerCharacterForNumber(int number) const;
	int GetWeightedIndex(const std::vector<int32> &weights, std::mt19937 &randGen) const;

	//TMap<int, DungeonsPlayerStatTracker> gameStats;
	TMap<int, DungeonsPlayerStatTracker> gameStats;
};
