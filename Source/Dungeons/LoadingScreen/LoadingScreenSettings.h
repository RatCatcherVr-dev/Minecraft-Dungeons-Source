// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Fonts/SlateFontInfo.h"
#include "SScaleBox.h"
#include "MoviePlayer.h"
#include "game/mission/MissionDef.h"
#include "game/LevelSettings.h"
#include "game/mission/theme/data/LoadingScreenStyle.h"

namespace game { namespace loadingscreen {

struct FFontLoadData {
	FFontLoadData(FSoftObjectPath, int, FName);

	FSoftObjectPath Path;
	int Size;
	FName Name;
};

/** Level specific data to be used in loading screen*/
struct FLoadingScreenMetaData {
	FLoadingScreenMetaData();
	FLoadingScreenMetaData(const FSoftObjectPath& levelImage, ELevelNames levelID, const FText& levelName, const FText& levelDescription);

	/** A level's loading screen image */
	/*UTexture2D* LevelImage;*/
	FSoftObjectPath LevelImage;

	/** A level's ID*/
	ELevelNames LevelID;

	/** A level's name to display during loading*/
	FText LevelName;

	/** A level's description to display during loading*/
	FText LevelDescription;
};

/** Data used to create loading screen*/
struct FLoadingScreenDescription {
	FLoadingScreenDescription(FLoadingScreenMetaData&& levelDesc
		, FFontLoadData&& levelFontPath, FFontLoadData&& travelingToFontPath, FFontLoadData&& difficultyFontPath, FFontLoadData&& threatLevelFontPath, FFontLoadData&& threatLevelPlusFontPath, const FLoadingScreenStyle& style);

	FLoadingScreenMetaData LevelMetaData;

	FSlateFontInfo LevelNameFont;

	FSlateFontInfo TravelingToFont;

	FSlateFontInfo DifficultyFont;

	FSlateFontInfo ThreatLevelFont;

	FSlateFontInfo ThreatLevelPlusFont;

	/** The scaling type to apply to images. */
	TEnumAsByte<EStretch::Type> ImageStretch;

	/** The tips to display on the load screen. */
	TArray<FText> Tips;

	/** The size of the tip before it's wrapped to the next line. */
	float TipWrapAt;

	/** The settings to display during loading screen*/
	FLevelSettings LevelSettings;

	FLoadingScreenStyle Style;

	float LoadingPlateDisplayDelay;
};

}}