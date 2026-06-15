#include "Dungeons.h"
#include "BaseMenuPlayerController.h"
#include "DungeonsGameInstance.h"
#include "game/LevelSettings.h"
#include "online/sessions/OnlineUtil.h"
#include "DungeonsGameState.h"
#include "lovika/LovikaLevelActor.h"
#include <HAL/ConsoleManager.h>
#include <SlateApplication.h>
#include "LoadingScreen/LoadingScreenInitializer.h"
#include "DungeonsUserManagement.h"

ABaseMenuPlayerController::ABaseMenuPlayerController(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
	, OnDestroySessionCompletionDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ABaseMenuPlayerController::OnDestroySessionComplete)) 
{

}

void ABaseMenuPlayerController::BeginPlay() {
	Super::BeginPlay();
	UWorld* world = GetWorld();

	auto Sessions = online::getSessionInterface();
	if (!Sessions.IsValid()) {
		UE_LOG(LogOnline, Log, TEXT("Session interface is not available"));
		return;
	}

	if (!ClearSession(Sessions)) {
		UE_LOG(LogOnline, Log, TEXT("No existing session to destroy"));
	}

	if (auto gi = Cast<UDungeonsGameInstance>(world->GetGameInstance())) {
		gi->GetFriendsInterface()->SetPresenceStatus(online::EmptyPresence);
		gi->GetFriendsInterface()->UpdateFriendsList();
	}
	
}

void ABaseMenuPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	auto* gi = Cast<UDungeonsGameInstance>(GetWorld()->GetGameInstance());
	gi->GetFriendsInterface()->SetPresenceStatus(EndPlayReason == EEndPlayReason::Quit ? online::EmptyPresence : gi->Configuration.GetLevelDisplayName());
}

void ABaseMenuPlayerController::OnGamepadActiveChanged(bool GamepadActive)
{
	APlayerControllerBase::OnGamepadActiveChanged(GamepadActive);
	if (GamepadActive)
	{
		bShowMouseCursor = false;
		bEnableMouseOverEvents = false;
		FSlateApplication::Get().OnCursorSet();
	}
	else
	{
		bShowMouseCursor = true;
		bEnableMouseOverEvents = true;
	}
}

void ABaseMenuPlayerController::StartOfflineGame() {
	StartGame(false, true);
}

void ABaseMenuPlayerController::StartOnlineGame(bool IsPrivate) {
	StartGame(true, IsPrivate);
}

void ABaseMenuPlayerController::StartGame(bool HostGame, bool IsPrivate) {
	UCharacterSaveData* characterSaveData = GetSaveData();
	if (!characterSaveData)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to start game, no player profile selected."));
		return;

	}	

	const auto loadLobby = characterSaveData->HasCompletedAnyLevel();

	auto mapLoadType = loadLobby ? EMapLoadType::StartLobbySession : EMapLoadType::StartIngameSession;
	const FLevelSettings levelSettings = loadLobby ? levelsettingsutil::generateLobbySettings(GetWorld()) : levelsettingsutil::generateTutorialSettings(GetWorld());
	if (!HostGame) {
		if (loadLobby) //D11.PS
		{
			mapLoadType = EMapLoadType::OpenLobby;
		}
		else
		{
			mapLoadType = EMapLoadType::OpenIngame;
		}
	}

	CurrentLevelSettings = levelSettings;
	CurrentMapLoadType = static_cast<uint8>(mapLoadType);

	auto* gi = Cast<UDungeonsGameInstance>(GetWorld()->GetGameInstance());
	int UserID = gi->GetUserManager()->GetInitialUserSystemId();
	if (auto saveState = gi->ReadGlobalSaveState(UserID)) //D11.PS - Protect against a null pointer
	{
		saveState->Save(UserID); // #D11.CM - Save our global state before loading.
	}
	gi->BeginLoadingScreenWithTravel({ CurrentLevelSettings, static_cast<EMapLoadType>(CurrentMapLoadType), {}, IsPrivate }, UDungeonsGameInstance::fDefaultFadeOutTime, UDungeonsGameInstance::fDefaultFadeInTime);
}

bool ABaseMenuPlayerController::ClearSession(TSharedPtr<online::Crossplay::Session> Sessions) {
	if (FNamedOnlineSession* existingSession = Sessions->GetNamedSession(DungeonsGameSessionName)) {
		UE_LOG(LogOnline, Log, TEXT("Destroying existing Session In State: %s"), EOnlineSessionState::ToString(existingSession->SessionState));
		Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompletionDelegateHandle);
		OnDestroySessionCompletionDelegateHandle = Sessions->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompletionDelegate);
		Sessions->DestroySession(DungeonsGameSessionName);
		return true;
	}
	return false;
}

void ABaseMenuPlayerController::GetSeamlessTravelActorList(bool bToEntry, TArray<AActor*>& ActorList) {
	Super::GetSeamlessTravelActorList(bToEntry, ActorList);
	ActorList.Add(GetGameInstance<UDungeonsGameInstance>()->GetAudioMusicManager());

}

void ABaseMenuPlayerController::OnDestroySessionComplete(FName sessionName, bool bWasSuccessful) {
	if (auto sessions = online::getSessionInterface()) {
		sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompletionDelegateHandle);
	}

	if (bWasSuccessful) {
		UE_LOG(LogOnline, Log, TEXT("Successfully destroyed session"));
	}
	else {
		UE_LOG(LogOnline, Log, TEXT("Failed to destroy session"));
	}
}
