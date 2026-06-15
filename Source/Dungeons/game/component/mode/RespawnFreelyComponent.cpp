#include "Dungeons.h"
#include "RespawnFreelyComponent.h"
#include "DungeonsGameState.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "DungeonsGameMode.h"


void URespawnFreelyComponent::BeginPlay() {
	Super::BeginPlay();
	if (const auto gamemode = Cast<ADungeonsGameMode>(GetOwner())) {
		gamemode->OnPlayerCharacterAdded.AddUObject(this, &URespawnFreelyComponent::OnPlayerCharacterAdded);
	}
}

void URespawnFreelyComponent::OnPlayerCharacterAdded(APlayerCharacter* player) {
	player->OnDeath.AddUObject(this, &URespawnFreelyComponent::RespawnPlayerCharacter, player);
	if (player->GetAliveState() == EAliveState::Dead) {
		RespawnPlayerCharacter(player);
	}
}

void URespawnFreelyComponent::RespawnPlayerCharacter(APlayerCharacter* player) {
	if(!player->IsRespawning()) {
		player->RespawnPlayerMulticast();
	}
}

ADungeonsGameState* URespawnFreelyComponent::GetGameState() const {
	const auto gameState = Cast<ADungeonsGameMode>(GetOwner())->GetGameState<ADungeonsGameState>();
	check(gameState && "Missing GameState in DungeonsGameMode");
	return gameState;
}