// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "Dungeons.h"
#include "Dungeons/LoadingScreen/LoadingScreenSettings.h"

#include "UObject/ConstructorHelpers.h"
#include "Engine/Font.h"


namespace game { namespace loadingscreen {

FFontLoadData::FFontLoadData(FSoftObjectPath path, int size, FName name)
	: Path(std::move(path))
	, Size(size)
	, Name(std::move(name)) {
}

FLoadingScreenMetaData::FLoadingScreenMetaData(const FSoftObjectPath& levelImage, ELevelNames levelID, const FText& levelName, const FText& levelDescription)
	: LevelImage(levelImage)
	, LevelID(levelID)
	, LevelName(levelName)
	, LevelDescription(levelDescription)
{}

FLoadingScreenMetaData::FLoadingScreenMetaData()
	: LevelImage(FSoftObjectPath())

{}

FSlateFontInfo LoadFont(const FFontLoadData& fontData) {
	if (auto font = fontData.Path.TryLoad()) {
		return FSlateFontInfo(font, fontData.Size, fontData.Name);
	}
	UE_LOG(LogDungeons, Warning, TEXT("Failed to load font"));
	return FSlateFontInfo();
}

FLoadingScreenDescription::FLoadingScreenDescription(FLoadingScreenMetaData&& levelDesc, FFontLoadData&& levelFont, FFontLoadData&& travelingToFont, FFontLoadData&& difficultyFont, FFontLoadData&& threatLevelFont, FFontLoadData&& threatLevelPlusFont, const FLoadingScreenStyle& style)
	: LevelMetaData(levelDesc)
	, Style(style)
	, ImageStretch(EStretch::ScaleToFit)
{
	if (!IsRunningDedicatedServer())
	{
		LevelNameFont = LoadFont(levelFont);
		TravelingToFont = LoadFont(travelingToFont);
		DifficultyFont = LoadFont(difficultyFont);
		ThreatLevelFont = LoadFont(threatLevelFont);
		ThreatLevelPlusFont = LoadFont(threatLevelPlusFont);
	}
}

}}
