#pragma once
#include <CoreMinimal.h>
#include <NoExportTypes.h>
#include "TooltipStyle.generated.h"

USTRUCT(BlueprintType)
struct FTooltipStyle {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor TextShadowColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.8f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor BackgroundColor = FLinearColor(0.004777f, 0.006995f, 0.01096f, 0.8);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector2D Offset = FVector2D(0.0f,0.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool AlwaysVisible = false;
};
