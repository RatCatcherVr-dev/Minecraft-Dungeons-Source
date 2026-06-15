#pragma once

#include <CoreMinimal.h>
#include "MissionMarkerStyle.h"
#include "ExtraChallengeStyle.h"
#include <Sound/SoundCue.h>
#include <SoftObjectPtr.h>
#include "TooltipStyle.h"
#include "MissionThemeMarkerStyle.generated.h"

USTRUCT(BlueprintType)
struct FMissionThemeMarkerStyle {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FMissionMarkerStyle Button;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FMissionMarkerStyle ButtonLocked;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FExtraChallengeStyle ExtraChallenge;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<USoundCue> AmbienceOverride;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FTooltipStyle TooltipStyle;

	void appendTexturePaths(TArray<TSoftObjectPtr<UTexture2D>>& paths) const {
		Button.appendTexturePaths(paths);
		ButtonLocked.appendTexturePaths(paths);
		ExtraChallenge.appendTexturePaths(paths);
	};
};