#pragma once
#include <CoreMinimal.h>
#include "ExtraChallengeStyle.h"
#include "game/difficulty/Difficulty.h"
#include "game/difficulty/ThreatLevel.h"
#include "LoadingScreenStyle.generated.h"

USTRUCT(BlueprintType)
struct FLoadingScreenStyle {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Badge;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BadgeScale = 0.3f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<EGameDifficulty, TSoftObjectPtr<UTexture2D>> Difficulty;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor DifficultyColor = { 1.0f, 1.0f, 1.0f, 1.0f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector2D DifficultyShadowOffset = {0.f,0.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor DifficultyShadowColor = {0.0f, 0.0f, 0.0f, 0.5f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> ThreatLevelFlag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor ThreatLevelShadowColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FExtraChallengeStyle ExtraChallenge;

	void appendTexturePaths(TArray<TSoftObjectPtr<UTexture2D>>& paths) const {
		if (Badge.IsValid()) {
			paths.Add(Badge);
		}
		if (ThreatLevelFlag.IsValid()) {
			paths.Add(ThreatLevelFlag);
		}
		paths.Append(algo::map_tarray(Difficulty, RETLAMBDA(it.Value)));
		ExtraChallenge.appendTexturePaths(paths);
		ExtraChallenge.appendTexturePaths(paths);
	};
};