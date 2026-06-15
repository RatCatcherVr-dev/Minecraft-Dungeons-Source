#include "Dungeons.h"
#include "DownedRevivePossibleComponent.h"
#include "DungeonsGameState.h"
#include "DungeonsGameMode.h"

void UDownedRevivePossibleComponent::BeginPlay() {
	Super::BeginPlay();
	auto gamestate = GetGameState();
	if (!gamestate) {
		//Error state - no refresh possible
		return;
	}

	gamestate->SetReviveEnabled(true);
}

void UDownedRevivePossibleComponent::EndPlay(EEndPlayReason::Type EndPlayReason) {
	auto gamestate = GetGameState();
	if (!gamestate) {
		//Error state - no refresh possible
		return;
	}

	gamestate->SetReviveEnabled(false);
}

ADungeonsGameState* UDownedRevivePossibleComponent::GetGameState() const {
	const auto gameState = Cast<ADungeonsGameMode>(GetOwner())->GetGameState<ADungeonsGameState>();
	check(gameState && "Missing GameState in DungeonsGameMode");
	return gameState;
}