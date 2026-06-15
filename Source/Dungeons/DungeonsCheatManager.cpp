#include "Dungeons.h"
#include "DungeonsCheatManager.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/Game.h"


void UDungeonsCheatManager::ShowDebugInfo(bool toggleShow)
{
	UWorld* World = GetWorld();
	check(World);

	for (FConstPlayerControllerIterator PCIterator = World->GetPlayerControllerIterator(); PCIterator; ++PCIterator)
	{
		ABasePlayerController* PlayerController = Cast<ABasePlayerController>(*PCIterator);

		if (PlayerController)
		{
			PlayerController->ShowDebugInfo(toggleShow);
		}
	}
}

void UDungeonsCheatManager::Suicide()
{
	UE_LOG(LogTelemetry, Log, TEXT("SUICIDE"));
	if (const auto game = actorquery::getGame(GetWorld()))
	{
		game->suicide();
	}
}

void UDungeonsCheatManager::EndGame()
{
	UE_LOG(LogTelemetry, Log, TEXT("COMPLETING LEVEL"));
	if (const auto game = actorquery::getGame(GetWorld()))
	{
		game->forceEndGame();
	}
}

