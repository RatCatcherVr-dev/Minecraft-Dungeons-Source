#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/Canvas.h"
#include "HealthBars.generated.h"

UCLASS()
class DUNGEONS_API UHealthBars : public UObject {

	struct HealthBarAppearance {
		public:
		const FColor BarColor;
		const FColor BackgroundColor;
		const float Opacity;
		const float Scale;
	};

	GENERATED_UCLASS_BODY()

public:
	UHealthBars();

	void Draw(UCanvas* canvas, APlayerController* playerOwner, float dpi_scale, AHUD* hud);
	static const float MASTER_SCALE;

	static const FColor HEALTH_BAR_COLOR;
	static const FColor HEALTH_BAR_HEAL_COLOR;
	static const FColor HEALTH_BAR_DAMAGE_COLOR;
	static const FColor HEALTH_BAR_BEHIND_COLOR;

private:
		
	FVector2D HealthBarFrameOffset;
	FVector2D HealthBarOffset;
	FVector2D HealthBarSize;
	
	UPROPERTY()
	FCanvasIcon HealthBarFrameIcon;

	UPROPERTY()
	UTexture2D* HealthBarTexture;

};
