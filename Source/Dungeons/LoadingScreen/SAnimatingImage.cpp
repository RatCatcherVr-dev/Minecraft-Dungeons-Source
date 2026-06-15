// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Dungeons.h"
#include "Dungeons/LoadingScreen/SAnimatingImage.h"
#include "Rendering/DrawElements.h"


FFrameUpdater::FFrameUpdater() : PassedTime(0.0f) {}

FFrameUpdater::FFrameUpdater(FAnimationData data) : PassedTime(0.0f), AnimationData(std::move(data)) {}

void FFrameUpdater::AddPassedTime(float dt) {
	PassedTime += dt;
}

const FBox2D FFrameUpdater::GetCurrentRenderBox() const {
	auto timesMaxed = static_cast<int>(PassedTime / AnimationData.MaxTimeSeconds);

	float XCoord = 0.0f;
	float YCoord = 0.0f;

	if (timesMaxed > 0 && !AnimationData.bLoop) {
		XCoord = (AnimationData.RowCount - 1) * AnimationData.FrameWidth;
		YCoord = (AnimationData.ColumnCount - 1) * AnimationData.FrameHeight;
	} 
	else {
		const auto overtime = AnimationData.MaxTimeSeconds * static_cast<float>(timesMaxed);

		const auto frame = static_cast<int>((PassedTime - overtime) / AnimationData.FrameTimeSeconds);

		const auto row = frame / AnimationData.ColumnCount;
		const auto column = frame - (row * AnimationData.ColumnCount);

		XCoord = AnimationData.FrameWidth * column;
		YCoord = AnimationData.FrameHeight * row;
	}

	auto XCoordFraction = XCoord / AnimationData.SheetWidth;
	auto frameWidthFraction = AnimationData.FrameWidth / AnimationData.SheetWidth;

	auto YCoordFraction = YCoord / AnimationData.SheetHeight;
	auto frameHeightFraction = AnimationData.FrameHeight / AnimationData.SheetHeight;

	FVector2D min(XCoordFraction, YCoordFraction);
	FVector2D max(XCoordFraction + frameWidthFraction, YCoordFraction + frameHeightFraction);

	return FBox2D(min, max);
}



FVector2D SAnimatingImage::ComputeDesiredSize(float) const {
	const FSlateBrush* ImageBrush = Image.Get();
	if (ImageBrush != nullptr)
	{
		return ImageBrush->ImageSize;
	}
	return FVector2D::ZeroVector;
}

void SAnimatingImage::Construct(const FArguments& InArgs)
{
	Image = InArgs._Image;
	ColorAndOpacity = InArgs._ColorAndOpacity;
	FrameUpdater = FFrameUpdater(InArgs._AnimationData);
	TransformationSequence = InArgs._TransformationSequence;
	TransformationSequence.Start();
}

void SAnimatingImage::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) {
	SLeafWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	FSlateBrush* ImageBrush = Image.Get();

	FrameUpdater.AddPassedTime(InDeltaTime);
	ImageBrush->SetUVRegion(FrameUpdater.GetCurrentRenderBox());

	TransformationSequence.Update(InDeltaTime);
}

// Override SImage's OnPaint to draw animated
int32 SAnimatingImage::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const
{
	const FSlateBrush* ImageBrush = Image.Get();

	if ((ImageBrush != nullptr) && (ImageBrush->DrawAs != ESlateBrushDrawType::NoDrawType))
	{
		const bool bIsEnabled = ShouldBeEnabled(bParentEnabled);
		const ESlateDrawEffect DrawEffects = bIsEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

		auto& currentTransformCurve = TransformationSequence.GetCurrentTransformationCurve();

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
		FPaintGeometry PaintGeom = AllottedGeometry.ToPaintGeometry(ImageBrush->ImageSize, ImageLocalTransform);

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
