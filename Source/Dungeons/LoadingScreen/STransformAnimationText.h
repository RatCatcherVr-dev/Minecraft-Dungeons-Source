// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/CurveSequence.h"
#include "Styling/SlateColor.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Styling/CoreStyle.h"
#include <chrono>
#include <App.h>
#include "TransformationSequence.h"
#include <STextBlock.h>
#include <SlateTextLayout.h>
#include <SlateTextBlockLayout.h>

class FPaintArgs;
class FSlateWindowElementList;

/** A widget that displays an animated image.*/
class DUNGEONS_API STransformAnimationTextBlock : public SLeafWidget
{
public:
	SLATE_BEGIN_ARGS(STransformAnimationTextBlock)
		: _Text()
		, _TextStyle(&FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText"))
		, _Font()
		, _ColorAndOpacity()
		, _ShadowOffset()
		, _ShadowColorAndOpacity()
		, _HighlightColor()
		, _HighlightShape()
		, _HighlightText()
		, _WrapTextAt(0.0f)
		, _AutoWrapText(false)
		, _WrappingPolicy(ETextWrappingPolicy::DefaultWrapping)
		, _Margin()
		, _LineHeightPercentage(1.0f)
		, _Justification(ETextJustify::Left)
		, _MinDesiredWidth(0.0f)
		, _TextShapingMethod()
		, _TextFlowDirection()
		, _LineBreakPolicy()
		,_TransformationSequence()
	{
		_Clipping = EWidgetClipping::OnDemand;
	}

		/** The text displayed in this text block */
		SLATE_ATTRIBUTE(FText, Text)

		/** Pointer to a style of the text block, which dictates the font, color, and shadow options. */
		SLATE_STYLE_ARGUMENT(FTextBlockStyle, TextStyle)

		/** Sets the font used to draw the text */
		SLATE_ATTRIBUTE(FSlateFontInfo, Font)

		/** Text color and opacity */
		SLATE_ATTRIBUTE(FSlateColor, ColorAndOpacity)

		/** Drop shadow offset in pixels */
		SLATE_ATTRIBUTE(FVector2D, ShadowOffset)

		/** Shadow color and opacity */
		SLATE_ATTRIBUTE(FLinearColor, ShadowColorAndOpacity)

		/** The color used to highlight the specified text */
		SLATE_ATTRIBUTE(FLinearColor, HighlightColor)

		/** The brush used to highlight the specified text*/
		SLATE_ATTRIBUTE(const FSlateBrush*, HighlightShape)

		/** Highlight this text in the text block */
		SLATE_ATTRIBUTE(FText, HighlightText)

		/** Whether text wraps onto a new line when it's length exceeds this width; if this value is zero or negative, no wrapping occurs. */
		SLATE_ATTRIBUTE(float, WrapTextAt)

		/** Whether to wrap text automatically based on the widget's computed horizontal space.  IMPORTANT: Using automatic wrapping can result
			in visual artifacts, as the the wrapped size will computed be at least one frame late!  Consider using WrapTextAt instead.  The initial
			desired size will not be clamped.  This works best in cases where the text block's size is not affecting other widget's layout. */
		SLATE_ATTRIBUTE(bool, AutoWrapText)

		/** The wrapping policy to use */
		SLATE_ATTRIBUTE(ETextWrappingPolicy, WrappingPolicy)

		/** The amount of blank space left around the edges of text area. */
		SLATE_ATTRIBUTE(FMargin, Margin)

		/** The amount to scale each lines height by. */
		SLATE_ATTRIBUTE(float, LineHeightPercentage)

		/** How the text should be aligned with the margin. */
		SLATE_ATTRIBUTE(ETextJustify::Type, Justification)

		/** Minimum width that a text block should be */
		SLATE_ATTRIBUTE(float, MinDesiredWidth)

		/** Which text shaping method should we use? (unset to use the default returned by GetDefaultTextShapingMethod) */
		SLATE_ARGUMENT(TOptional<ETextShapingMethod>, TextShapingMethod)

		/** Which text flow direction should we use? (unset to use the default returned by GetDefaultTextFlowDirection) */
		SLATE_ARGUMENT(TOptional<ETextFlowDirection>, TextFlowDirection)

		/** The iterator to use to detect appropriate soft-wrapping points for lines (or null to use the default) */
		SLATE_ARGUMENT(TSharedPtr<IBreakIterator>, LineBreakPolicy)

		/** Transformation sequence */
		SLATE_ARGUMENT(FTransformationSequence, TransformationSequence)

		/** Called when this text is double clicked */
		SLATE_EVENT(FOnClicked, OnDoubleClicked)

	SLATE_END_ARGS()

	FVector2D ComputeDesiredSize(float) const override;

	/* Construct this widget */
	void Construct( const FArguments& InArgs );

	void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	int32 OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const override;

private:
	TSharedRef<FSlateTextLayout> CreateTransformTextLayout(SWidget* InOwner, const FTextBlockStyle& InDefaultTextStyle);

	/** Gets the current foreground color */
	FSlateColor GetColorAndOpacity() const;

	/** Gets the current font */
	FSlateFontInfo GetFont() const;

	/** Gets the current shadow offset */
	FVector2D GetShadowOffset() const;

	/** Gets the current shadow color and opacity */
	FLinearColor GetShadowColorAndOpacity() const;

	/** Gets the current highlight color */
	FLinearColor GetHighlightColor() const;

	/** Gets the current highlight shape */
	const FSlateBrush* GetHighlightShape() const;

	/** Get the computed text style to use with the text marshaller */
	FTextBlockStyle GetComputedTextStyle() const;

	/** The text displayed in this text block */
	TAttribute< FText > BoundText;

	/** The wrapped layout for this text block */
	TUniquePtr< FSlateTextBlockLayout > TextLayoutCache;

	/** Default style used by the TextLayout */
	FTextBlockStyle TextStyle;

	/** Sets the font used to draw the text */
	TAttribute< FSlateFontInfo > Font;

	/** Text color and opacity */
	TAttribute<FSlateColor> ColorAndOpacity;

	/** Drop shadow offset in pixels */
	TAttribute< FVector2D > ShadowOffset;

	/** Shadow color and opacity */
	TAttribute<FLinearColor> ShadowColorAndOpacity;

	TAttribute<FLinearColor> HighlightColor;

	TAttribute< const FSlateBrush* > HighlightShape;

	/** Highlight this text in the textblock */
	TAttribute<FText> HighlightText;

	/** Whether text wraps onto a new line when it's length exceeds this width; if this value is zero or negative, no wrapping occurs. */
	TAttribute<float> WrapTextAt;

	/** True if we're wrapping text automatically based on the computed horizontal space for this widget */
	TAttribute<bool> AutoWrapText;

	/** The wrapping policy we're using */
	TAttribute<ETextWrappingPolicy> WrappingPolicy;

	/** The amount of blank space left around the edges of text area. */
	TAttribute< FMargin > Margin;

	/** The amount to scale each lines height by. */
	TAttribute< ETextJustify::Type > Justification;

	/** How the text should be aligned with the margin. */
	TAttribute< float > LineHeightPercentage;

	/** Prevents the text block from being smaller than desired in certain cases (e.g. when it is empty) */
	TAttribute<float> MinDesiredWidth;

	/** The delegate to execute when this text is double clicked */
	FOnClicked OnDoubleClicked;

	FTransformationSequence TransFormationSequence;
};
