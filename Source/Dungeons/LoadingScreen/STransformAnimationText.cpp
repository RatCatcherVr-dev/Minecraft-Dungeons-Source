// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Dungeons/LoadingScreen/STransformAnimationText.h"
#include "Rendering/DrawElements.h"
#include <ReflectionMetadata.h>
#include <PlainTextLayoutMarshaller.h>
#include <SlateTextLayout.h>
#include <SlateTextLayoutFactory.h>
#include <SharedPointer.h>
#include <ISlateRun.h>
#include <ISlateRunRenderer.h>

FVector2D STransformAnimationTextBlock::ComputeDesiredSize(float LayoutScaleMultiplier) const {
	//SCOPE_CYCLE_COUNTER(Stat_SlateTextBlockCDS);

	// ComputeDesiredSize will also update the text layout cache if required
	const FVector2D TextSize = TextLayoutCache->ComputeDesiredSize(
		FSlateTextBlockLayout::FWidgetArgs(BoundText, HighlightText, WrapTextAt, AutoWrapText, WrappingPolicy, Margin, LineHeightPercentage, Justification),
		LayoutScaleMultiplier, GetComputedTextStyle()
	);

	return FVector2D(FMath::Max(MinDesiredWidth.Get(0.0f), TextSize.X), TextSize.Y);
}

void STransformAnimationTextBlock::Construct(const FArguments& InArgs) {
	TextStyle = *InArgs._TextStyle;

	HighlightText = InArgs._HighlightText;
	WrapTextAt = InArgs._WrapTextAt;
	AutoWrapText = InArgs._AutoWrapText;
	WrappingPolicy = InArgs._WrappingPolicy;
	Margin = InArgs._Margin;
	LineHeightPercentage = InArgs._LineHeightPercentage;
	Justification = InArgs._Justification;
	MinDesiredWidth = InArgs._MinDesiredWidth;

	Font = InArgs._Font;
	ColorAndOpacity = InArgs._ColorAndOpacity;
	ShadowOffset = InArgs._ShadowOffset;
	ShadowColorAndOpacity = InArgs._ShadowColorAndOpacity;
	HighlightColor = InArgs._HighlightColor;
	HighlightShape = InArgs._HighlightShape;

	OnDoubleClicked = InArgs._OnDoubleClicked;

	BoundText = InArgs._Text;
	
	// We use a dummy style here (as it may not be safe to call the delegates used to compute the style), but the correct style is set by ComputeDesiredSize
	TextLayoutCache = MakeUnique<FSlateTextBlockLayout>(this, FTextBlockStyle::GetDefault(), InArgs._TextShapingMethod, InArgs._TextFlowDirection, FCreateSlateTextLayout(), FPlainTextLayoutMarshaller::Create(), InArgs._LineBreakPolicy);
	TextLayoutCache->SetDebugSourceInfo(TAttribute<FString>::Create(TAttribute<FString>::FGetter::CreateLambda([this] { return FReflectionMetaData::GetWidgetDebugInfo(this); })));

	TransFormationSequence = InArgs._TransformationSequence;
	TransFormationSequence.Start();
}

void STransformAnimationTextBlock::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) {
	SLeafWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	TransFormationSequence.Update(InDeltaTime);
}

int32 STransformAnimationTextBlock::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const
{
	//SCOPE_CYCLE_COUNTER(Stat_SlateTextBlockOnPaint);

	auto& currentTransformCurve = TransFormationSequence.GetCurrentTransformationCurve();

	auto desiredSize = ComputeDesiredSize(1.0f);

	auto TextLocalTransform = FSlateLayoutTransform(
		currentTransformCurve.GetScale(),
		TransformPoint(currentTransformCurve.GetScale(), (-desiredSize / 2.0f)) +
		TransformPoint({ desiredSize.X / 2.0f, desiredSize.Y / 2.0f }, currentTransformCurve.GetPosition()));
	
	auto newGeometry = AllottedGeometry.MakeChild(AllottedGeometry.GetLocalSize(), TextLocalTransform);

	LayerId = TextLayoutCache->OnPaint(Args, newGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, ShouldBeEnabled(bParentEnabled));

	if (TextLayoutCache->GetDesiredSize().Y > GetDesiredSize().Y)
	{
		const_cast<STransformAnimationTextBlock*>(this)->Invalidate(EInvalidateWidget::Layout);
	}

	return LayerId;
}

FSlateFontInfo STransformAnimationTextBlock::GetFont() const
{
	return Font.IsSet() ? Font.Get() : TextStyle.Font;
}

FSlateColor STransformAnimationTextBlock::GetColorAndOpacity() const
{
	return ColorAndOpacity.IsSet() ? ColorAndOpacity.Get() : TextStyle.ColorAndOpacity;
}

FVector2D STransformAnimationTextBlock::GetShadowOffset() const
{
	return ShadowOffset.IsSet() ? ShadowOffset.Get() : TextStyle.ShadowOffset;
}

FLinearColor STransformAnimationTextBlock::GetShadowColorAndOpacity() const
{
	return ShadowColorAndOpacity.IsSet() ? ShadowColorAndOpacity.Get() : TextStyle.ShadowColorAndOpacity;
}

FLinearColor STransformAnimationTextBlock::GetHighlightColor() const
{
	return HighlightColor.IsSet() ? HighlightColor.Get() : TextStyle.HighlightColor;
}

const FSlateBrush* STransformAnimationTextBlock::GetHighlightShape() const
{
	return HighlightShape.IsSet() ? HighlightShape.Get() : &TextStyle.HighlightShape;
}


FTextBlockStyle STransformAnimationTextBlock::GetComputedTextStyle() const {
	auto& currentTransformCurve = TransFormationSequence.GetCurrentTransformationCurve();

	FTextBlockStyle ComputedStyle = TextStyle;
	ComputedStyle.SetFont(GetFont());
	ComputedStyle.SetColorAndOpacity(FSlateColor(GetColorAndOpacity().GetSpecifiedColor() * currentTransformCurve.GetColor()));
	ComputedStyle.SetShadowOffset(GetShadowOffset());
	ComputedStyle.SetShadowColorAndOpacity(FLinearColor(GetShadowColorAndOpacity() * currentTransformCurve.GetColor()));
	ComputedStyle.SetHighlightColor(GetHighlightColor());
	ComputedStyle.SetHighlightShape(*GetHighlightShape());
	return ComputedStyle;
}