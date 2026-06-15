// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "game/mission/MissionDef.h"

#include "CoreMinimal.h"
#include "game/levels.h"
#include "game/difficulty/Difficulty.h"
#include "LoadingScreenSettings.h"

enum class EGameDifficulty : uint8;
enum class ELoadingScreenType : uint8;

namespace game { namespace loadingscreen {

class LoadingScreenBuilder {
public:
	LoadingScreenBuilder();

	TSharedPtr<SWidget> SetupLoadingScreenWidget(const MissionDef& missionDef, const FLevelSettings& levelSettings, ELoadingScreenType screenType, float loadingPlateDisplayDelay, const FLoadingScreenStyle& style);
	FSoftObjectPath GetTexture(ELoadingScreenType type, ELevelNames level) const;
private:
	FLoadingScreenMetaData GetLoadingScreenMetaData(ELoadingScreenType type, ELevelNames level);
	
	FSoftObjectPath MenuTexture;
	FSoftObjectPath LobbyTexture;
};

}}

