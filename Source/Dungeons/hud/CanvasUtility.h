#pragma once

#include "CoreMinimal.h"
#include "game/item/ItemSlot.h"
#include "Runtime/Engine/Classes/Engine/Canvas.h"


class DUNGEONS_API CanvasUtility
{
public:
	static FCanvasIcon MakeIconFullTexture(UTexture2D* tex, FVector2D& outSize, float padding = 0.0f);
	static FCanvasIcon MakeIconFullTexture(UTexture2D* tex, float padding = 0.0f);
	static FVector ProjectLocationToCanvas(const FVector& worldLocation, float floatingOffset, UCanvas* canvas);
	static bool IsInToViewport(const FVector& projectedLocation, FVector2D viewportSize);
	static FVector2D ClampToViewport(const FVector& projectedLocation, FVector2D viewportSize);
	static FVector2D ProjectLocationToCanvasViewport(const FVector& worldLocation, float floatingOffset, UCanvas* canvas, FVector2D viewportSize);
	static FVector2D ClampToViewportEdge(const FVector2D& projectedPosition, FVector2D viewportSize, FVector2D screenEdgePadding);
};

