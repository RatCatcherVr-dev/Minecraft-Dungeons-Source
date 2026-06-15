#include "Dungeons.h"
#include "DungeonsGameInstance.h"
#include "ReconnectUtil.h"
#include "ReconnectStates.h"
#include "lovika/LovikaLevelActor.h"
#include "game/component/ReconnectComponent.h"
#include "DungeonsGameSession.h"

void reconnect::connect(const FString& guid, const APlayerController* pc) {
	auto* gameInstance = Cast<UDungeonsGameInstance>(pc->GetGameInstance());
	const auto isReconnecting = isReconnected(pc);
	gameInstance->GetReconnectStates().Connect(guid);
	UE_LOG(LogTemp, Log, TEXT("reconnectstate::Connect, %s is %s"), *guid, isReconnecting ? TEXT("reconnecting.") : TEXT("connecting for the first time."));
}

void reconnect::disconnect(const APlayerCharacter* pc) {
	TOptional<RestoreState> restoreState;
	if (pc->IsLoadedInLevel()) {
		restoreState = RestoreState::CreateFromPlayerCharacter(pc);
	}
	auto* gameInstance = Cast<UDungeonsGameInstance>(pc->GetGameInstance());
	gameInstance->GetReconnectStates().Disconnect(getReconnectComponent(pc)->GetGuid(), restoreState);
}

bool reconnect::canRestore(const APlayerCharacter* playerCharacter) {
	const auto* gi = Cast<UDungeonsGameInstance>(playerCharacter->GetGameInstance());
	const auto* pc = Cast<APlayerController>(playerCharacter->GetController());
	return gi->GetReconnectStates().GetRestoreState(getReconnectComponent(pc)->GetGuid()).IsSet();
}

void reconnect::restore(APlayerCharacter* playerCharacter) {
	UE_LOG(LogMultiplayer, Log, TEXT("Reconnect: restore"));
	const auto* rc = getReconnectComponent(playerCharacter);
	auto& reconnectStates = Cast<UDungeonsGameInstance>(playerCharacter->GetGameInstance())->GetReconnectStates();
	if (const auto& rs = reconnectStates.GetRestoreState(rc->GetGuid())) {
		UE_LOG(LogMultiplayer, Log, TEXT("Reconnect: restore for guid %s"), *rc->GetGuid());
		rs->ApplyToPlayerCharacter(playerCharacter);
		reconnectStates.ClearRestoreState(rc->GetGuid());
	}
}

void reconnect::completeReconnect(const APlayerController* playerController) {
	const auto* rc = getReconnectComponent(playerController);
	UE_LOG(LogMultiplayer, Log, TEXT("Reconnect: completing reconnect for guid %s"), *rc->GetGuid());
	auto& reconnectStates = Cast<UDungeonsGameInstance>(playerController->GetGameInstance())->GetReconnectStates();
	reconnectStates.CompleteReconnect(rc->GetGuid());
}

bool reconnect::isFirstLocalPlayerControllerReconnected(UWorld* world) {
	const auto* gameInstance = Cast<UDungeonsGameInstance>(world->GetGameInstance());
	const auto localPlayerController = gameInstance->GetFirstLocalPlayerController();
	return localPlayerController ? isReconnected(localPlayerController) : false;
}

bool reconnect::isReconnected(const APlayerController* pc) {
	const auto* rc = getReconnectComponent(pc);
	if (rc == nullptr) {
		UE_LOG(LogMultiplayer, Warning, TEXT("Reconnect: Checking if a player is reconnected, but the player controller has no reconnect component."));
		return false;
	}
	const auto* instance = Cast<UDungeonsGameInstance>(pc->GetGameInstance());
	return instance->GetReconnectStates().IsReconnect(rc->GetGuid());
}

UReconnectComponent* reconnect::getReconnectComponent(const APlayerCharacter* playerCharacter) {
	return getReconnectComponent(Cast<APlayerController>(playerCharacter->GetController()));
}

UReconnectComponent* reconnect::getReconnectComponent(const APlayerController* playerController) {
	return playerController->FindComponentByClass<UReconnectComponent>();
}

TArray<FString> reconnect::getReconnectableGuids(const UWorld* world) {
	const auto* gi = Cast<UDungeonsGameInstance>(world->GetGameInstance());
	auto* dgs = Cast<ADungeonsGameSession>(world->GetAuthGameMode()->GameSession);
	TArray<FString> result;
	for (auto& guid : gi->GetReconnectStates().GetGuids()) {
		if (gi->GetReconnectStates().CanReconnect(guid) && !dgs->IsKicked(guid))
			result.Add(guid);
	}
	return result;
}

FString reconnect::getGuid(const APlayerController* playerController) {
	if (playerController) {
		if (const auto* playerState = playerController->PlayerState) {
			if (playerState->UniqueId.IsValid()) {
				return playerState->UniqueId->ToString();
			}
		}
	}
	return "";
}
