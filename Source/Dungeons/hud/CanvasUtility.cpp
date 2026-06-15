#include "Dungeons.h"
#include "Engine.h"
#include "CanvasUtility.h"



FCanvasIcon CanvasUtility::MakeIconFullTexture(UTexture2D* tex, FVector2D& outSize, float padding) {
	//Padded grab possible to fix issues with wrapping UV artifacts.
	//Textures need to have padding-equivalent transparent buffer
	outSize = FVector2D(tex->GetSizeX() - padding * 2, tex->GetSizeY() - padding * 2);
	return UCanvas::MakeIcon(tex, padding, padding, tex->GetSizeX() - padding, tex->GetSizeY() - padding);
};

FCanvasIcon CanvasUtility::MakeIconFullTexture(UTexture2D* tex, float padding) {
	//Padded grab possible to fix issues with wrapping UV artifacts.
	//Textures need to have padding-equivalent transparent buffer
	return UCanvas::MakeIcon(tex, padding, padding, tex->GetSizeX() - padding, tex->GetSizeY() - padding);
};

FVector CanvasUtility::ProjectLocationToCanvas(const FVector& location, float floatingOffset, UCanvas* canvas) {
	const auto offset = FVector::UpVector * floatingOffset;

	auto projectedLocation = canvas->Project(location + offset);
	if (projectedLocation.Z == 0) {
		projectedLocation *= -1.f;
	}
	
	return projectedLocation;
};

bool CanvasUtility::IsInToViewport(const FVector& projectedLocation, FVector2D viewportSize) {	
	return !(projectedLocation.X < 0 || projectedLocation.X >= viewportSize.X || projectedLocation.Y < 0 || projectedLocation.Y >= viewportSize.Y);
};

FVector2D CanvasUtility::ClampToViewport(const FVector& projectedLocation, FVector2D viewportSize) {
	const auto viewportHalf = viewportSize * .5f;
	const auto viewportRadius = viewportHalf.Size();

	auto projectedShort = (projectedLocation - FVector{ viewportHalf, 0.f }).GetClampedToMaxSize2D(viewportRadius) + FVector{ viewportHalf, 0.f };
	return FVector2D(projectedShort.X, projectedShort.Y);
};

FVector2D CanvasUtility::ProjectLocationToCanvasViewport(const FVector& location, float floatingOffset, UCanvas* canvas, FVector2D viewportSize) {
	auto projectedLocation = ProjectLocationToCanvas(location, floatingOffset, canvas);
	return ClampToViewport(projectedLocation, viewportSize);
};

FVector2D CanvasUtility::ClampToViewportEdge(const FVector2D& projectedPosition, FVector2D viewportSize, FVector2D screenEdgePadding) {
	FVector2D projectedClamped{
		FMath::Clamp(projectedPosition.X, screenEdgePadding.X, viewportSize.X - screenEdgePadding.X),
		FMath::Clamp(projectedPosition.Y, screenEdgePadding.Y, viewportSize.Y - screenEdgePadding.Y)
	};
	return projectedClamped;
};