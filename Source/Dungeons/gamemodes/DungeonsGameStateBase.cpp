#include "Dungeons.h"
#include "game/actor/character/player/BasePlayerState.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "DungeonsGameStateBase.h"


bool ADungeonsGameStateBase::IsLobby() const {
	return false;
}

void ADungeonsGameStateBase::AddPlayerController(ABasePlayerController* playerController){
}

void ADungeonsGameStateBase::RemovePlayerController(ABasePlayerController* playerController){
}

void ADungeonsGameStateBase::AddPlayerState(APlayerState* PlayerState) {
	Super::AddPlayerState(PlayerState);
	if (auto ps = Cast<ABasePlayerState>(PlayerState)) {
		OnPlayerStateAdded.Broadcast(ps);
	}
	OnPlayersCountChanged.Broadcast();
}

void ADungeonsGameStateBase::RemovePlayerState(APlayerState* PlayerState) {
	Super::RemovePlayerState(PlayerState);
	if (auto ps = Cast<ABasePlayerState>(PlayerState)) {
		OnPlayerStateRemoved.Broadcast(ps);
	}
	OnPlayersCountChanged.Broadcast();
}

TArray<ABasePlayerState*> ADungeonsGameStateBase::GetPlayerStates() const {
	TArray<ABasePlayerState*> states;
	for (auto ps : PlayerArray) {
		if (auto basePs = Cast<ABasePlayerState>(ps)) {
			states.Add(basePs);
		}
	}
	return states;
}

int ADungeonsGameStateBase::GetPlayersCount() const {
	return PlayerArray.FilterByPredicate([](const APlayerState* ps) {
		return ps->IsA(ABasePlayerState::StaticClass());
	}).Num();
}