#pragma once

#include "CoreMinimal.h"
#include "game/item/ItemSlot.h"
#include "Runtime/Engine/Classes/Engine/Canvas.h"
#include "HotBarBackground.generated.h"


UCLASS()
class DUNGEONS_API UHotBarBackground : public UObject {
	GENERATED_UCLASS_BODY()

public:
	void Draw(UCanvas* canvas, APlayerController* playerOwner, float dpi_scale, float dt, AHUD* hud);

	void SetVisible(bool is_visible);

	void SetOffset(FVector2D newOffset);

	void SetScale(float scale);

	void SetCoopOffset(int index, FVector2D newOffset);

	void SetHotbarSize(FVector2D newSize);

protected:

	static const float MASTER_SCALE;
	static const float TEXTURE_PADDING;

	static const FVector2D HOT_BAR_POSITION;
	static const FVector2D HOT_BAR_ALIGNMENT;
	static FVector2D HOT_BAR_SIZE;
	static const FVector2D COOP_HOT_BAR_SIZE;
	static const FLinearColor HOT_BAR_COLOR;
	static const FVector2D COOP_HOT_BAR_ALIGNMENTS[4];

	static float SCALE_MULTIPLIER;
	static const float HORIZONTAL_PADDING;
	static const float INNER_HOTBAR_PADDING;
	
	UPROPERTY()
	bool visible = false;

	UPROPERTY()
	FVector2D coopOffsets[4];
};

