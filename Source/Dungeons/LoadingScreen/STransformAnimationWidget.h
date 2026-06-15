// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/CurveSequence.h"
#include "Styling/SlateColor.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Styling/CoreStyle.h"
#include <chrono>
#include <App.h>
#include "Widgets/SLeafWidget.h"
#include "TransformationSequence.h"

class FPaintArgs;
class FSlateWindowElementList;

/** A widget that displays an animated image.*/
class DUNGEONS_API STransformAnimationWidget : public SLeafWidget
{
public:
	SLATE_BEGIN_ARGS(STransformAnimationWidget)
		: _Image( FCoreStyle::Get().GetDefaultBrush() )
		, _ColorAndOpacity(FLinearColor::White)
		, _TransformationSequence(FTransformationSequence())
		{}

		/** Image resource */
		SLATE_ATTRIBUTE( const FSlateBrush*, Image )

		/** Color and opacity */
		SLATE_ARGUMENT(FSlateColor, ColorAndOpacity)

		/** Transformation sequence */
		SLATE_ARGUMENT(FTransformationSequence, TransformationSequence )

	SLATE_END_ARGS()

	FVector2D ComputeDesiredSize(float) const override;

	/* Construct this widget */
	void Construct( const FArguments& InArgs );

	void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	int32 OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const override;

private:
	TAttribute<const FSlateBrush*> Image;

	TAttribute<FSlateColor> ColorAndOpacity;

	FTransformationSequence TransFormationSequence;
};
