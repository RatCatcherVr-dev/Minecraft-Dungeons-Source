#pragma once

#include "game/levels.h"

class APlayerCharacter;
class UReconnectComponent;

namespace reconnect {
	void connect(const FString& guid, const APlayerController* pc);
	void disconnect(const APlayerCharacter* pc);
	bool isFirstLocalPlayerControllerReconnected(UWorld*);

	bool isReconnected(const APlayerController*);

	bool canRestore(const APlayerCharacter*);
	void restore(APlayerCharacter*);
	void completeReconnect(const APlayerController*);

	UReconnectComponent* getReconnectComponent(const APlayerCharacter*);
	UReconnectComponent* getReconnectComponent(const APlayerController*);
	TArray<FString> getReconnectableGuids(const UWorld*);
	FString getGuid(const APlayerController*);
}
