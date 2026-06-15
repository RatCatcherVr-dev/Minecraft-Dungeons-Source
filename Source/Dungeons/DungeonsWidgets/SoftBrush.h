// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SoftBrush.generated.h"

USTRUCT(BlueprintType)
struct FSoftBrush
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool matchSize = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayThumbnail = "true"))
	TSoftObjectPtr<UTexture2D> texture;
};
