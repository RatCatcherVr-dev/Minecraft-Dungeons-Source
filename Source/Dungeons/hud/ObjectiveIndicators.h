#pragma once

#include "CoreMinimal.h"
#include <Engine/Texture2D.h>
#include <functional>
#include "game/actor/OxygenSourceComponent.h"
#include "ObjectiveIndicators.generated.h"

class AHUD;


namespace objectiveindicators {
	const float outlineTimeLoopProgress(UWorld* world, const float loopDuration = 5.f, const float timeLoopBias = 0.15f, const float loopFractionOffset = 0.0f);
	const float outlineScale(float prg);
	const float outlineOpacity(float prg);
	const float jumpOffset(UWorld* world);
	const float notifyOpacity(UWorld* world);
}

USTRUCT()
struct DUNGEONS_API FObjectiveIndicatorIcon {
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	UTexture2D* tex = nullptr;

	FVector2D size = FVector2D(0.f, 0.f);

public:
	FObjectiveIndicatorIcon() {}
	FObjectiveIndicatorIcon(UTexture2D* texture) {
		tex = texture;
		size = FVector2D(texture->GetSizeX(), texture->GetSizeY());
	}

	const FVector2D& GetSize() const {
		return size;
	}
	const UTexture2D* GetTexture() const {
		return tex;
	}

	void Draw(AHUD* hud, const FVector2D& drawPosition, float scale, float rotation, const FLinearColor& color) const;
};


struct IndicatorState {
	float rotation;
	float distance;	
	float degreeToObjective;
	const struct FObjectiveIndicator* indicator;
	FVector2D drawPosition;
	bool inViewport;
};


USTRUCT()
struct DUNGEONS_API FObjectiveIndicator {
	GENERATED_BODY()

	using DrawFunction = std::function<void(UWorld*, AHUD*, const IndicatorState&, float)>;
	UPROPERTY()
	FVector2D Bounds;
	
	DrawFunction Draw;
	const FVector2D& GetBounds() const {
		return Bounds;
	};	
};


UCLASS()
class DUNGEONS_API UObjectiveIndicators : public UObject {
	GENERATED_UCLASS_BODY()
		
public:
	void Draw(UCanvas* canvas, APlayerController* playerOwner, float dpi_scale, AHUD* hud);

	UFUNCTION(BlueprintNativeEvent)
	bool IsInsideCoopHotbar(FVector2D& pos);

	void ClearOxygenSource();

private:

	static const FBox2D VIEWPORT_HIDE_PORTRAIT_REGION;
	static const float VIEWPORT_HIDE_HEIGHT_FRACTION;
	static const float VIEWPORT_HIDE_WIDTH_FRACTION;
	
	void UpdateIndicatorState(UCanvas* canvas, AHUD* hud, const FVector2D& viewportSize, const FVector& indicatorLocation, const FVector& playerLocation, const FObjectiveIndicator& indicatorBig, const FObjectiveIndicator& indicatorSmall, const float scale, IndicatorState& OutIndicatorState);
	void DrawIndicator(UWorld* world, UCanvas* canvas, AHUD* hud, const FVector2D& viewportSize, const FVector& indicatorLocation, const FVector& playerLocation, const FObjectiveIndicator& indicatorBig, const FObjectiveIndicator& indicatorSmall, const float scale, IndicatorState& OutIndicatorState);

	FVector GetNearestOxygenLocation(FVector);

	static const float MASTER_SCALE;
	static const float SWORD_ROTATION_OFFSET_DEGREES;
	static const float WORLD_OBJECTIVE_PROJECTION_OFFSET;
	static const float WORLD_HINT_TARGET_PROJECTION_OFFSET;
	static const float WORLD_MERCHANT_PROJECTION_OFFSET;
	static const float WORLD_NOTIFY_PROJECTION_OFFSET;
	static const float WORLD_DOOR_PROJECTION_OFFSET;
	static const float WORLD_OXYGEN_PROJECTION_OFFSET;

	float smoothRotationBg = 0.0f;

	UPROPERTY()
	FObjectiveIndicatorIcon ObjectiveMarker;
	UPROPERTY()
	FObjectiveIndicatorIcon ObjectiveReached;
	UPROPERTY()
	FObjectiveIndicatorIcon ObjectiveOutline;
	UPROPERTY()
	FObjectiveIndicatorIcon ObjectiveDirection;	
	UPROPERTY()
	FObjectiveIndicatorIcon ObjectiveSmall;

	UPROPERTY()
	FObjectiveIndicatorIcon NotifyOutline;
	UPROPERTY()
	FObjectiveIndicatorIcon NotifyInline;
	UPROPERTY()
	FObjectiveIndicatorIcon NofitySmall;

	UPROPERTY()
	FObjectiveIndicator ObjectiveIndicatorBig;
	UPROPERTY()
	FObjectiveIndicator ObjectiveIndicatorSmall;
	UPROPERTY()
	
	FObjectiveIndicator NotifyIndicatorBig;
	UPROPERTY()
	FObjectiveIndicator NotifyIndicatorSmall;

	UPROPERTY()
	FObjectiveIndicatorIcon MerchantBubble;
	UPROPERTY()
	FObjectiveIndicatorIcon MerchantExclamation;

	UPROPERTY()
	FObjectiveIndicator MerchantIndicatorBig;
	UPROPERTY()
	FObjectiveIndicator MerchantIndicatorSmall;

	UPROPERTY()
	FObjectiveIndicatorIcon OxygenMarkerBig;
	UPROPERTY()
	FObjectiveIndicatorIcon OxygenMarkerSmall;
	UPROPERTY()
	FObjectiveIndicator OxygenIndicatorBig;
	UPROPERTY()
	FObjectiveIndicator OxygenIndicatorSmall;

	UPROPERTY()
	class AGameBP* gameBp = nullptr;

	IndicatorState& GetOrCreateIndicatorState(int index);

	TMap<int, IndicatorState> IndicatorStates;

	TWeakObjectPtr<AOxygenSourceComponent> CurrentNearestOxygenSource;
};
