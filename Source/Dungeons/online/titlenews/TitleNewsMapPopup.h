#pragma once

#include "game/mission/theme/MissionTheme.h"
#include "TitleNewsMapPopup.generated.h"

class UTexture2DDynamic;

USTRUCT(BlueprintType)
struct FTitleNewsMapPopup
{
	GENERATED_BODY()
	FString Id;
	
	UPROPERTY(BlueprintReadOnly)
	FText Title;

	UPROPERTY(BlueprintReadOnly)
	FText Body;

	UPROPERTY(BlueprintReadOnly)
	UTexture2DDynamic* HeaderImage = nullptr;

	UPROPERTY(BlueprintReadOnly)
	UTexture2DDynamic* ThumbnailImage = nullptr;

	UPROPERTY(BlueprintReadOnly)
	EMissionTheme Theme = EMissionTheme::Invalid;

	UPROPERTY(BlueprintReadOnly)
	bool HasCountdown = false;

	UPROPERTY(BlueprintReadOnly)
	FDateTime EndTime;

	TOptional<FString> ImageURL;

	TOptional<FString> ThumbnailURL;
};
