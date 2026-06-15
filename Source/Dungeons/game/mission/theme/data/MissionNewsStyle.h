#pragma once
#include <CoreMinimal.h>
#include <NoExportTypes.h>
#include "Sound/SoundCue.h"
#include "MissionNewsStyle.generated.h"

USTRUCT(BlueprintType)
struct FMissionNewsStyle {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor BorderColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor GradientColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<USoundCue> PopupSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<USoundCue> AmbienceOverride;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<USoundCue> MusicOverride;
};