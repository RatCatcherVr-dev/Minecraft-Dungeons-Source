// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#include "DifficultyTokenInstance.h"
#include "Dungeons/game/Game.h"
#include "Dungeons/DungeonsGameMode.h"
#include "game/GameBP.h"

void ADifficultyTokenInstance::Activate(const FPredictionKey& predictionKey)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (AGameBP* game = actorquery::getFirstActor<AGameBP>(GetWorld()))
		{
			game->IncrementRaidDifficulty();
		}
	}
}