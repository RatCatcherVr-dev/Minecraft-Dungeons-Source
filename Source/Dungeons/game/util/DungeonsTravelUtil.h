#pragma once

#include "CoreMinimal.h"
#include "game/LevelSettings.h"

enum class EGameDifficulty : uint8;
enum class ELevelNames : uint8;

namespace travelutil {

void OpenMenu(const UWorld*);
void OpenLevel(const UWorld*, const FLevelSettings&);
void ServerTravelToLobby(UWorld*, const FLevelSettings&);
void ServerTravelToGameMap(UWorld*, const FLevelSettings&);
void ClientTravel(APlayerController*, const FString& url, const FLevelSettings&);

};
