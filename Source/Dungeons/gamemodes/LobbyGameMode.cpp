#include "Dungeons.h"
#include "LobbyGameMode.h"
#include "LobbyGameState.h"
#include "game/actor/character/player/BasePlayerController.h"
#include <Runtime/UMG/Public/Blueprint/UserWidget.h>


ALobbyGameMode::ALobbyGameMode() {
	GameStateClass = ALobbyGameState::StaticClass();
}