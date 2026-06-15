#pragma once

#include "../DungeonsGameState.h"
#include "LobbyGameState.generated.h"

UCLASS(minimalapi)
class ALobbyGameState : public ADungeonsGameState {
	GENERATED_BODY()
public:
	bool IsLobby() const override;
private:
};
