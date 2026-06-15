#pragma once

#include "Dungeons.h"
#include "util/EnumUtil.h"
#include "game/mission/MissionDefs.h"
#include "StringTableCore.h"
#include <StringTableRegistry.h>
#include "DungeonsGameInstance.h"

namespace game {
	namespace loadingscreen {

#define LOCTEXT_NAMESPACE "LoadingScreen"

static const TMap<ELoadingScreenType, FText> GenericLevelLabels = {
	TPair<ELoadingScreenType, FText>(ELoadingScreenType::Menu, LOCTEXT("level_Menu", "Menu")),
	TPair<ELoadingScreenType, FText>(ELoadingScreenType::Lobby, LOCTEXT("level_Lobby", "Camp"))
};

static const TMap<ELoadingScreenType, FText> GenericLevelDescriptions = {
	TPair<ELoadingScreenType, FText>(ELoadingScreenType::Menu, LOCTEXT("leveldesc_Menu", "Back to the beginning")),
	TPair<ELoadingScreenType, FText>(ELoadingScreenType::Lobby, LOCTEXT("leveldesc_Lobby", "A Place for rest and reflection"))
};


static const FText s_TravelToText = LOCTEXT("load_travelto", "Traveling to");
static const FText s_LoadingScreenShortText = LOCTEXT("load_levelname_short", "{level}");

static const FText& LoadingScreenTravelingTo() { return s_TravelToText; }
static const FText& LoadingScreenLevel() { return s_LoadingScreenShortText; }

static const FText& GetLoadingScreenName(ELoadingScreenType type, ELevelNames levelId = ELevelNames::Invalid)
{
	if (type == ELoadingScreenType::Level)
	{		
		const auto mission = missions::getChecked(levelId);
		if(mission){
			return mission->getNameText();
		}
	}

	if (GenericLevelLabels.Contains(type))
		return GenericLevelLabels[type];

	return FText::GetEmpty();

}

static const FText& GetLoadingScreenDescription(ELoadingScreenType type, ELevelNames levelId = ELevelNames::Invalid)
{
	if (type == ELoadingScreenType::Level)
	{		
		const auto mission = missions::getChecked(levelId);
		if(mission){
			return mission->getStoryTitleText();
		}
		return FText::GetEmpty();
	}

	if (GenericLevelLabels.Contains(type))
		return GenericLevelDescriptions[type];

	return FText::GetEmpty();
}

#undef LOCTEXT_NAMESPACE
} // end of namespace loadingscreen
}
