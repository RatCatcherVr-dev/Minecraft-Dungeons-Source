#pragma once

#include "Difficulty.h"
#include "ExtraChallenge.h"
#include "EndlessStruggle.h"
#include <Kismet/BlueprintFunctionLibrary.h>
#include "ThreatLevel.h"
#include "DifficultyUtil.generated.h"

namespace game { struct FDifficulty; }

UCLASS(BlueprintType)
class UDifficultyUtil : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static int GetStartingLivesForDifficulty(EGameDifficulty Difficulty);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText getDifficultyDisplayName(EGameDifficulty Difficulty);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText getDifficultyPlusDisplayName(EGameDifficulty Difficulty);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText getThreatLevelDisplayName(EThreatLevel ThreatLevel);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText getThreatCompletedRequirementText(EThreatLevel ThreatLevel);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText getEndlessStruggleDisplayName(FEndlessStruggle EndlessStruggle);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText getDifficultyThreatLevelDisplayName(EGameDifficulty Difficulty, EThreatLevel ThreatLevel);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText getDifficultyEndlessStruggleDisplayName(EGameDifficulty Difficulty, FEndlessStruggle EndlessStruggle);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText GetUnlockText(EGameDifficulty Difficulty);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText GetUnlockRequirementText(EGameDifficulty Difficulty);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText GetExtraChallengeDispalyName(EExtraChallenge Challenge);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText GetUnlockExplainerText(EGameDifficulty Difficulty);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FEndlessStruggleConfiguration GetEndlessStruggleConfiguration(FEndlessStruggle level);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static int GetNumThreatLevelsInDifficulty(EGameDifficulty Difficulty);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static int GetNumEndlessStrugglesInDifficulty(EGameDifficulty Difficulty);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static int GetNumThreatLevelsAndEndlessStrugglesInDifficulty(EGameDifficulty Difficulty);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static int GetTotalNumThreatLevels();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static int GetTotalNumEndlessStruggles();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static int GetTotalNumThreatLevelsAndEndlessStruggles();

	static game::FDifficulty GetDifficultyWithEmergentDifficulty(const game::FDifficulty& difficulty, TOptional<FEmergentDifficulty> EmergentDifficulty);
};
