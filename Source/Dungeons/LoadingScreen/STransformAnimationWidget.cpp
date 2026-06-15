// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Dungeons/LoadingScreen/STransformAnimationWidget.h"
#include "Rendering/DrawElements.h"
#include <Engine.h>

FVector2D STransformAnimationWidget::ComputeDesiredSize(float) const {
	const FSlateBrush* ImageBrush = Image.Get();
	if (ImageBrush != nullptr)
	{
		return ImageBrush->ImageSize;
	}
	return FVector2D::ZeroVector;
}

void STransformAnimationWidget::Construct(const FArguments& InArgs)
{
	Image = InArgs._Image;
	ColorAndOpacity = InArgs._ColorAndOpacity;
	TransFormationSequence = InArgs._TransformationSequence;
	TransFormationSequence.Start();
}

void STransformAnimationWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) {
	SLeafWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	TransFormationSequence.Update(InDeltaTime);
}

// Override OnPaint to draw animated
int32 STransformAnimationWidget::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const
{
	const FSlateBrush* ImageBrush = Image.Get();

	if ((ImageBrush != nullptr) && (ImageBrush->DrawAs != ESlateBrushDrawType::NoDrawType))
	{
		const bool bIsEnabled = ShouldBeEnabled(bParentEnabled);
		const ESlateDrawEffect DrawEffects = bIsEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

		auto& currentTransformCurve = TransFormationSequence.GetCurrentTransformationCurve();

		const FLinearColor FinalColorAndOpacity(
			InWidgetStyle.GetColorAndOpacityTint() *
			ColorAndOpacity.Get().GetColor(InWidgetStyle) *
			currentTransformCurve.GetColor() *
			ImageBrush->GetTint(InWidgetStyle));

		auto ImageLocalTransform = FSlateLayoutTransform(
			currentTransformCurve.GetScale(),
			TransformPoint(currentTransformCurve.GetScale(), (-ImageBrush->ImageSize / 2.0f)) +
			TransformPoint({ ImageBrush->ImageSize.X / 2.0f, ImageBrush->ImageSize.Y / 2.0f }, currentTransformCurve.GetPosition()));

		auto widgetCenterPos = AllottedGeometry.GetAbsolutePosition() + AllottedGeometry.GetAbsoluteSize() / 2.0f;
		auto currentRenderBox = currentTransformCurve.GetClipBox();

		OutDrawElements.PushClip(FSlateClippingZone(FSlateRect({ widgetCenterPos.X + currentRenderBox.Min.X, widgetCenterPos.Y + currentRenderBox.Min.Y }, { widgetCenterPos.X + currentRenderBox.Max.X, widgetCenterPos.Y + currentRenderBox.Max.Y })));
		FPaintGeometry PaintGeom = AllottedGeometry.ToPaintGeometry(AllottedGeometry.GetLocalSize(), ImageLocalTransform);

		FSlateDrawElement::MakeBox(OutDrawElements,
			LayerId, 
			PaintGeom, 
			ImageBrush,
			DrawEffects,
			FinalColorAndOpacity);

		OutDrawElements.PopClip();
	}
	return LayerId;
}