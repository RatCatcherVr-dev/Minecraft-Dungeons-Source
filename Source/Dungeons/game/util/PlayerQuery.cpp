#include "Dungeons.h"
#include "PlayerQuery.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/player/PlayerControllerBase.h"
#include <Engine/World.h>
#include <Engine/GameInstance.h>
#include <Engine/LocalPlayer.h>
#include <GameFramework/PlayerController.h>

namespace playerquery {

APlayerCharacter* getPrimaryPlayerCharacter(UWorld* world) {
	if (auto* controller = world->GetGameInstance()->GetPrimaryPlayerController()) {
		return Cast<APlayerCharacter>(controller->GetPawn());
	}
	return nullptr;
}

APlayerCharacter* getFirstLocalPlayerCharacter(UWorld* world) {
	if (auto* controller = getFirstLocalPlayerController(world)) {
		return Cast<APlayerCharacter>(controller->GetPawn());
	}
	return nullptr;
}

APlayerControllerBase* getFirstLocalPlayerController(UWorld* world) {
	if (auto* localPlayer = world->GetFirstLocalPlayerFromController()) {
		return Cast<APlayerControllerBase>(localPlayer->GetPlayerController(nullptr));
	}
	return nullptr;
}

}
