#pragma once

#include <CoreMinimal.h>
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

#include "ChanceInfoIconWidget.generated.h"


UCLASS()
class DUNGEONS_API UChanceInfoIconWidget : public UUserWidget {
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly,  BlueprintReadOnly, Category = "Dungeons")
	TSoftObjectPtr<UTexture2D> iconTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dungeons")	
	FVector2D iconOffset = { 0.0f, 0.0f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dungeons")
	float iconScale = 1.0f;
};
