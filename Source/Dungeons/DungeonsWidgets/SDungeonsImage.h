// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Attribute.h"
#include "Input/Reply.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Styling/SlateColor.h"
#include "Styling/CoreStyle.h"
#include "Widgets/SLeafWidget.h"
#include <SImage.h>

class FPaintArgs;
class FSlateWindowElementList;

/**
 * Implements a widget that displays an image with a desired width and height.
 */
class DUNGEONS_API SDungeonsImage
	: public SImage
{
public:

	void SetSnapToPixels(bool Snap);

public:

	// SWidget overrides
	virtual int32 OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const override;
	
private:
	bool SnapToPixels = true;
};
