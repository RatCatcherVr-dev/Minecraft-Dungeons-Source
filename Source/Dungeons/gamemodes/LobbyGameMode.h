#pragma once

#include "DungeonsGameMode.h"
#include "game/actor/character/loot/LootActor.h"
#include "LobbyGameMode.generated.h"


UCLASS(minimalapi)
class ALobbyGameMode : public ADungeonsGameMode {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	float arrowReplenishTime = 2.0f;

	UPROPERTY(EditDefaultsOnly)
	float soulReplenishTime = 2.0f;

public:
	ALobbyGameMode();

	float GetArrowReplenishTime() const { return arrowReplenishTime; };
	float GetSoulReplenishTime() const { return soulReplenishTime; };

};
