#include "Dungeons.h"
#include "DungeonsTravelUtil.h"
#include "GameFramework/PlayerController.h"
#include "DungeonsGameInstance.h"
#include "Assets/DungeonsAssetManager.h"

namespace travelutil {

FDungeonsConfiguration& getConfig(const UWorld* world) {
	return Cast<UDungeonsGameInstance>(world->GetGameInstance())->Configuration;
}

void OpenMenu(const UWorld* world) {
	UE_LOG(LogDungeons, Log, TEXT("UDungeonsTravelUtil::OpenMenu"));
	UGameplayStatics::OpenLevel(world, "menu", true);
}

void OpenLevel(const UWorld* world, const FLevelSettings& levelSettings) {
	UE_LOG(LogDungeons, Log, TEXT("UDungeonsTravelUtil::OpenLevel"));
	auto* gameInstance = Cast<UDungeonsGameInstance>(world->GetGameInstance());
	gameInstance->Configuration.PrepareTravel(levelSettings);
	UGameplayStatics::OpenLevel(world, FName(*levelSettings.getUnrealMapName()), true, "listen");
}

void ServerTravelToLobby(UWorld* world, const FLevelSettings& levelSettings) {
	const auto serverType = world->GetNetMode() == ENetMode::NM_ListenServer ? FString("?listen") : FString("");
	const auto url = FString("/Game/GameModes/Lobby/Lobby")
		+ FString("?game=/Game/GameModes/Lobby/BP_LobbyGameMode.BP_LobbyGameMode_C")
		+ serverType;

	UE_LOG(LogDungeons, Log, TEXT("UDungeonsTravelUtil::ServerTravelToLobby with url '%s'"), *url);
	getConfig(world).PrepareTravel(levelSettings);
	world->ServerTravel(url, true, false);
}

void ServerTravelToGameMap(UWorld* world, const FLevelSettings& levelSettings) {
	const auto mapName = "ingame";
	const auto serverType = world->GetNetMode() == ENetMode::NM_ListenServer ? FString("?listen") : FString("");
	const auto url = FString("/Game/GameModes/")
		+ mapName
		+ FString("/")
		+ mapName
		+ FString("?game=/Game/GameModes/Ingame/BP_IngameGameMode.BP_IngameGameMode_C")
		+ serverType;

	UE_LOG(LogDungeons, Log, TEXT("UDungeonsTravelUtil::ServerTravelToGameMap to url '%s' with level name '%s'"), *url, *levelSettings.getLevelDisplayName().ToString());
	getConfig(world).PrepareTravel(levelSettings);
	world->ServerTravel(url, true, false);
}

void ClientTravel(APlayerController* playerController, const FString& url, const FLevelSettings& levelSettings) {
	UE_LOG(LogDungeons, Log, TEXT("UDungeonsTravelUtil::ClientTravel to url '%s' with level name '%s'"), *url, *levelSettings.getLevelDisplayName().ToString());
	getConfig(playerController->GetWorld()).PrepareTravel(levelSettings);
	playerController->ClientTravel(url, TRAVEL_Absolute);
}

}
