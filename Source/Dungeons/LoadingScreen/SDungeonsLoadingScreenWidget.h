// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <SCompoundWidget.h>
#include "LoadingScreenSettings.h"
#include <DeferredCleanupSlateBrush.h>
#include <SlateBrush.h>
#include "LoadingScreen/MutableDeferredCleanupSlateBrush.h"
#include "game/mission/variation/LevelVariationType.h"

namespace game { namespace loadingscreen {

class SDungeonsLoadingScreen : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SDungeonsLoadingScreen) {}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const FLoadingScreenDescription& ScreenDescription);

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	
	void ClearAllReferences();
	void ClearBrushRefences(TSharedPtr<FDeferredCleanupInterface> brush);

private:
	float GetDPIScale() const;

private:
	TSharedPtr<FDeferredCleanupSlateBrush> LoadingScreenBrush;
	TSharedPtr<FDeferredCleanupSlateBrush> LoadingPlateBrush;
	TSharedPtr<FDeferredCleanupSlateBrush> LoadingPlateFrameBrush;
	TSharedPtr<FDeferredCleanupSlateBrush> LoadingPlateDividerBrush;
	TSharedPtr<FDeferredCleanupSlateBrush> DifficultyBadgeBrush;
	TSharedPtr<FDeferredCleanupSlateBrush> DifficultyBrush;
	TSharedPtr<FDeferredCleanupSlateBrush> ThreatLevelBadgeBrush;	
	TSharedPtr<FDeferredCleanupSlateBrush> RightWingBrush;
	TSharedPtr<FMutableDeferredCleanupSlateBrush> LoadingAnimationBrush;
	
	float LastComputedDPIScale;

public:
	/** Dtor ensures that active timer handles are UnRegistered with the SlateApplication. */
	virtual ~SDungeonsLoadingScreen();
};

}}