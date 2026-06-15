#include "Dungeons.h"
#include "LobbyActor.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "util/Algo.h"
#include <UnrealNetwork.h>

#include "DungeonsGameStateBase.h"

const float ALobbyActor::MISSION_SELECTION_TIMEOUT = 30.0f;

ALobbyActor::ALobbyActor() {
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bAlwaysRelevant = true;
	SetReplicates(true);
}

const FLevelRequest& ALobbyActor::RequestLevel(int32 byPlayerId, APlayerCharacter* requester, const FLevelSettings& levelSettings) {
	mTimeout = MISSION_SELECTION_TIMEOUT;
	ABasePlayerState* requesterPlayerState = requester->GetDungeonsBasePlayerState();

	mRequest = FLevelRequest {
		byPlayerId,
		requesterPlayerState->GetPlayerDisplayName(),
		levelSettings,
		requesterPlayerState
	};

	OnRep_RequestChanged();

	SetIsRequestingLevel(true);

	SetActorTickEnabled(true);

	return mRequest;
}

void ALobbyActor::SetIsRequestingLevel(bool IsRequesting) {
	mIsRequestingLevel = IsRequesting;
	OnRep_IsRequestingLevelChanged();
}

void ALobbyActor::OnRep_RequestChanged() {
	FString LocalPlayerName = mRequest.PlayerName;
	if (const auto* bps = mRequest.PlayerState) {
		LocalPlayerName = bps->GetPlayerDisplayName();
	}
	OnRequestChanged.Broadcast({ mRequest.PlayerId, LocalPlayerName, mRequest.LevelSettings, mRequest.PlayerState });
}

void ALobbyActor::OnRep_IsRequestingLevelChanged() {
	OnRequestingLevelChanged.Broadcast(mIsRequestingLevel);
}


void ALobbyActor::Tick(float DeltaSecs) {

	//Animated timer because this is used for synchronized countdowns across clients.
	mTimeout -= DeltaSecs;

	if (mTimeout <= 0.0f) {
		mTimeout = 0.0f;
		SetActorTickEnabled(false);		
		if(OnMissionSelectionTimedOut.IsBound()){
			OnMissionSelectionTimedOut.Broadcast();
		}
	}
}

bool ALobbyActor::IsRequestingLevel() const {
	return mIsRequestingLevel;
}

bool ALobbyActor::TryAccept(int numPlayers) {
	const auto acceptedPlayerCount = [this] { return algo::count_if(InstanceTracker<APlayerCharacter>::GetList(GetWorld()), [](const APlayerCharacter* player) {
		const auto state = player->GetDungeonsBasePlayerState();
		return state && state->IsReady();
	}); };

	if (IsRequestingLevel() && acceptedPlayerCount() >= numPlayers) {
		SetIsRequestingLevel(false);
		return true;
	}
	return false;
}

void ALobbyActor::Reject() {
	SetIsRequestingLevel(false);
}

const FLevelRequest& ALobbyActor::GetRequest() const {
	return mRequest;
}

void ALobbyActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALobbyActor, mTimeout);
	DOREPLIFETIME(ALobbyActor, mRequest);
	DOREPLIFETIME(ALobbyActor, mIsRequestingLevel);
}


float ALobbyActor::GetTimeout() const {
	return mTimeout;
}

