// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "Dungeons.h"
#include "LoadingScreenBuilder.h"
#include "LoadingScreenSettings.h"
#include "game/levels.h"
#include <DeclarativeSyntaxSupport.h>
#include "SDungeonsLoadingScreenWidget.h"
#include "game/difficulty/Difficulty.h"
#include "game/mission/LoadingScreenDefs.h"


namespace game { namespace loadingscreen {

static TPair<ELevelNames, FSoftObjectPath> GetPair(ELevelNames level, FSoftObjectPath textureDef) {
	return TPair<ELevelNames, FSoftObjectPath>(level, textureDef);
}

// NOTE: Level names and descriptions are defined and managed in MissionDefs.cpp
// Avoid adding user facing text macros in this file!
LoadingScreenBuilder::LoadingScreenBuilder()
	: MenuTexture("/Game/UI/Materials/LoadingScreens/loadingGame.loadingGame")
	, LobbyTexture("/Game/UI/Materials/LoadingScreens/Loading_Screen_Lobby.Loading_Screen_Lobby")
{
}

FSoftObjectPath LoadingScreenBuilder::GetTexture(ELoadingScreenType type, ELevelNames level) const {
	
	switch (type)
	{
	case ELoadingScreenType::Menu:
		return MenuTexture;
	case ELoadingScreenType::Lobby:
		return LobbyTexture;
	case ELoadingScreenType::Level:
		if (auto mission = missions::getChecked(level)) {
			auto texturePath = mission->getLoadingScreenTexturePath();
			if (texturePath.IsSet()) {
				return texturePath.GetValue();
			}
		}		
	default:
		// falls back to menu texture, if level is not found in lookup:
		return MenuTexture;
	}
}

FLoadingScreenMetaData LoadingScreenBuilder::GetLoadingScreenMetaData(ELoadingScreenType type, ELevelNames level) {

	ensure(type < ELoadingScreenType::Count);
	return FLoadingScreenMetaData(
		GetTexture(type, level)
		, level
		, game::loadingscreen::GetLoadingScreenName(type, level)
		, game::loadingscreen::GetLoadingScreenDescription(type, level)
	);
}

TSharedPtr<SWidget> LoadingScreenBuilder::SetupLoadingScreenWidget(const MissionDef& missionDef, const FLevelSettings& levelSettings, ELoadingScreenType screenType, float loadingPlateDisplayDelay, const FLoadingScreenStyle& style) {

	FSoftObjectPath fontpath("/Game/Fonts/Minecraft.Minecraft");
	FSoftObjectPath fiveFontpath("/Game/Fonts/NewFive.NewFive");
	
	FLoadingScreenDescription description(std::move(GetLoadingScreenMetaData(screenType, missionDef.level())), { std::move(fontpath), 56, FName("Ten") }, { std::move(fontpath), 22, FName("NotoBold") }, { std::move(fontpath), 22, FName("Ten") }, { std::move(fiveFontpath), 14, FName("FiveBold") } , { std::move(fontpath), 15, FName("Ten") }, style);
	
	description.LevelSettings = levelSettings;
	description.LoadingPlateDisplayDelay = loadingPlateDisplayDelay;

	return SNew(SDungeonsLoadingScreen, description);
}

}}