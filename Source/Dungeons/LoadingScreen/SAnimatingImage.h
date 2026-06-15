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
#include "LoadingScreen/TransformationSequence.h"

class FPaintArgs;
class FSlateWindowElementList;

struct FAnimationData {

	FAnimationData() 
		: bLoop(true), 
		  ColumnCount(2),
		  RowCount(1),
		  TotalFrameCount(2),
		  FrameTimeSeconds(0.1f), 
		  MaxTimeSeconds(FrameTimeSeconds * TotalFrameCount),
		  FrameWidth(100.0f), 
		  TotalFrameWidth(FrameWidth * ColumnCount),
		  FrameHeight(100.0f),
		  TotalFrameHeight(FrameHeight * RowCount),
		  SheetWidth(0),
		  SheetHeight(0) {
	}

	FAnimationData(bool loop, int columnCount, int rowCount, int totalFrameCount, float frameTime, float frameWidth, float frameHeight, float sheetWidth, float sheetHeight)
		: bLoop(loop), 
		  ColumnCount(columnCount),
		  RowCount(rowCount),
		  TotalFrameCount(totalFrameCount),
		  FrameTimeSeconds(frameTime), 
		  MaxTimeSeconds(FrameTimeSeconds * TotalFrameCount), 
		  FrameWidth(frameWidth), 
		  TotalFrameWidth(FrameWidth * ColumnCount),
		  FrameHeight(frameHeight),
		  TotalFrameHeight(FrameHeight * RowCount),
		  SheetWidth(sheetWidth),
		  SheetHeight(sheetHeight) {
	}

	bool bLoop;
	int ColumnCount;
	int RowCount;
	int TotalFrameCount;
	float FrameTimeSeconds;
	float MaxTimeSeconds;
	float FrameWidth;
	float TotalFrameWidth;
	float FrameHeight;
	float TotalFrameHeight;
	float SheetWidth;
	float SheetHeight;
};

struct FFrameUpdater {
	FAnimationData AnimationData;

	FFrameUpdater();
	FFrameUpdater(FAnimationData data);

	const FBox2D GetCurrentRenderBox() const;

	void AddPassedTime(float dt);
private:
	float PassedTime;
};

/** A widget that displays an animated image.*/
class DUNGEONS_API SAnimatingImage : public SLeafWidget
{
public:
	SLATE_BEGIN_ARGS(SAnimatingImage)
		: _Image( FCoreStyle::Get().GetDefaultBrush() )
		, _ColorAndOpacity(FLinearColor::White)
		, _AnimationData(FAnimationData())
		, _TransformationSequence()
		{}

		/** Image resource */
		SLATE_ATTRIBUTE( FSlateBrush*, Image )

		/** Color and opacity */
		SLATE_ATTRIBUTE( FSlateColor, ColorAndOpacity )

		/** Data for the animation */
		SLATE_ARGUMENT( FAnimationData, AnimationData )

		/** Data for the animation */
		SLATE_ARGUMENT(FTransformationSequence, TransformationSequence)

	SLATE_END_ARGS()

	FVector2D ComputeDesiredSize(float) const override;

	/* Construct this widget */
	void Construct( const FArguments& InArgs );

	void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	int32 OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const override;

private:
	TAttribute<FSlateBrush*> Image;

	TAttribute<FSlateColor> ColorAndOpacity;

	FFrameUpdater FrameUpdater;

	FTransformationSequence TransformationSequence;
};
