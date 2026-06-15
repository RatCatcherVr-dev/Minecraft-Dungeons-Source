#include "Dungeons.h"
#include "JoinDungeonsSessionCallbackProxy.h"
#include "Engine.h"
#include "FindSessionsCallbackProxy.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "DungeonsGameInstance.h"
#include "game/util/DungeonsTravelUtil.h"
#include "SessionSettings.h"
#include "OnlineUtil.h"
#include "LoadingScreen/LoadingScreenInitializer.h"
#include "online/crossplay/Session.h"
#include "DungeonsUserManagement.h"
#include "online/crossplay/CrossplayOSS.h"

UJoinDungeonsSessionCallbackProxy::UJoinDungeonsSessionCallbackProxy(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
	, WorldContextObject(nullptr)
	, PlayerControllerWeakPtr(nullptr)
	, OnJoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &UJoinDungeonsSessionCallbackProxy::OnCompleted))
	, OnDestroySessionCompletionDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &UJoinDungeonsSessionCallbackProxy::OnDestroySessionComplete)) {
}

UJoinDungeonsSessionCallbackProxy* UJoinDungeonsSessionCallbackProxy::JoinDungeonsSession(UObject* WorldContextObject, class APlayerController* PlayerController, const FBlueprintSessionResult& SearchResult) {
	return JoinDungeonsSession(WorldContextObject ,PlayerController, SearchResult.OnlineResult);
}

UJoinDungeonsSessionCallbackProxy* UJoinDungeonsSessionCallbackProxy::JoinDungeonsSession(UObject* WorldContextObject, class APlayerController* PlayerController, const FOnlineSessionSearchResult& SearchResult) {
	UJoinDungeonsSessionCallbackProxy* Proxy = NewObject<UJoinDungeonsSessionCallbackProxy>();
	Proxy->PlayerControllerWeakPtr = PlayerController;
	Proxy->SearchResult = SearchResult;
	Proxy->WorldContextObject = WorldContextObject;
	return Proxy;
}

void UJoinDungeonsSessionCallbackProxy::Activate() {
	UWorld* GameWorld = GEngine->GetWorldFromContextObject(WorldContextObject.Get(), EGetWorldErrorMode::ReturnNull);
	if (!PlayerControllerWeakPtr.IsValid()) {
		// D11.DH
		// Fix for issue where you can't join after accepting an invite whilst suspended on Xbox which caused a softlock
		UDungeonsGameInstance* GameInstance = Cast<UDungeonsGameInstance>(GEngine->GameViewport->GetGameInstance());
		if (GameInstance)
		{
			PlayerControllerWeakPtr = GameInstance->GetFirstLocalPlayerController();
			if (!PlayerControllerWeakPtr.IsValid())
			{
				CallFailEvent(ESessionFailureReason::InvalidPlayerController, ESessionJoinFailureReason::UnknownError);
				return;
			}
		}
		else
		{
			CallFailEvent(ESessionFailureReason::InvalidPlayerController, ESessionJoinFailureReason::UnknownError);
			return;
		}
	}

	auto OnlineSub = online::getCrossplayOss();
	if (!OnlineSub)
	{
		CallFailEvent(ESessionFailureReason::JoinSessionFailed, ESessionJoinFailureReason::UnknownError);
		return;
	}

	auto Sessions = online::getSessionInterface();
	if (!Sessions.IsValid()) {
		CallFailEvent(ESessionFailureReason::SessionInterfaceUnavailable, ESessionJoinFailureReason::UnknownError);
		return;
	}

	Sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);

	const auto identity = OnlineSub->GetIdentityIF();
	if (!identity.IsValid()) {
		return;
	}

	int initialUser = GameWorld->GetGameInstance<UDungeonsGameInstance>()->GetUserManager()->GetInitialUser();
	LocalPlayerId = identity->GetUniquePlayerId(initialUser, SearchResult.Session.OwningUserId->GetType());

	if (identity->GetLoginStatus(initialUser) != ELoginStatus::LoggedIn) {
		UE_LOG_ONLINE(Log, TEXT("[JoinDungeonSession] The identity is not logged in"));
		return;
	}

	auto gameInstance = PlayerControllerWeakPtr->GetGameInstance<UDungeonsGameInstance>();

	SessionSettings sessionSettings(SearchResult);

	auto mapName = sessionSettings.GetLevelSettings().getUnrealMapName();
	auto mapLoadType = mapName == "Lobby" ? EMapLoadType::JoinLobbySession : EMapLoadType::JoinIngameSession;

	UE_LOG_ONLINE(Log, TEXT("[JoinDungeonSession] mapName='%s', LoadType='%s'"), *mapName, mapLoadType == EMapLoadType::JoinLobbySession ? *FString("Lobby") : *FString("InGame"));

	auto loadingScreenReady = [this, Sessions, initialUser] {
		if (!ClearAndJoinSession(Sessions)) {
			OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);
			Sessions->JoinSession(initialUser, DungeonsGameSessionName, SearchResult);
		}
	};
	gameInstance->BeginLoadingScreenWithTravel({ sessionSettings.GetLevelSettings(), mapLoadType, loadingScreenReady, false }, UDungeonsGameInstance::fDefaultFadeOutTime, UDungeonsGameInstance::fDefaultFadeInTime);
}

void UJoinDungeonsSessionCallbackProxy::CallFailEvent(ESessionFailureReason Reason, ESessionJoinFailureReason ExtendedReason) {
	UWorld* world = GEngine->GetWorldFromContextObject(WorldContextObject.Get(), EGetWorldErrorMode::ReturnNull);
	if (world)
	{
		auto gameInstance = world->GetGameInstance<UDungeonsGameInstance>();
		gameInstance->JoinSessionFailed(Reason, ExtendedReason);
	}
	JoinSessionFailed.Broadcast(Reason, ExtendedReason);
}

void UJoinDungeonsSessionCallbackProxy::OnCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result) {
	UWorld* world = GEngine->GetWorldFromContextObject(WorldContextObject.Get(), EGetWorldErrorMode::ReturnNull);

	auto Sessions = online::getSessionInterface();
	if (!Sessions.IsValid()) {
		CallFailEvent(ESessionFailureReason::SessionInterfaceUnavailable, ESessionJoinFailureReason::UnknownError);
		return;
	}

	Sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);

	if (!PlayerControllerWeakPtr.IsValid()) {
		// D11.DH
		// Fix for issue where you can't join after accepting an invite whilst suspended on Xbox which caused a softlock
		UDungeonsGameInstance* GameInstance = Cast<UDungeonsGameInstance>(GEngine->GameViewport->GetGameInstance());
		if (GameInstance)
		{
			PlayerControllerWeakPtr = GameInstance->GetFirstLocalPlayerController();
			if (!PlayerControllerWeakPtr.IsValid())
			{
				CallFailEvent(ESessionFailureReason::InvalidPlayerController, ESessionJoinFailureReason::UnknownError);
				return;
			}
		}
		else
		{
			CallFailEvent(ESessionFailureReason::InvalidPlayerController, ESessionJoinFailureReason::UnknownError);
			return;
		}
		
		
	}

	if (Result == EOnJoinSessionCompleteResult::Success) {
		FString connectionString;
		if (Sessions->GetResolvedConnectString(DungeonsGameSessionName, connectionString)) {
			SessionSettings sessionSettings(SearchResult);

			travelutil::ClientTravel(PlayerControllerWeakPtr.Get(), connectionString, sessionSettings.GetLevelSettings());

			JoinSessionCompleted.Broadcast();
			if (LocalPlayerId->IsValid()) {
				Sessions->RegisterPlayer(SessionName, *LocalPlayerId, false);
			}
		} else {
			UE_LOG_ONLINE(Log, TEXT("[JoinDungeonSession] Failed get resolved connection address (SessionName='%s')"), *DungeonsGameSessionName.ToString());
			CallFailEvent(ESessionFailureReason::ConnectionStringResolutionFailed, ESessionJoinFailureReason::UnknownError);
		}
	} else {
		switch (Result) {
		case EOnJoinSessionCompleteResult::Type::SessionIsFull:
			CallFailEvent(ESessionFailureReason::JoinSessionFailed, ESessionJoinFailureReason::SessionIsFull);
			break;
		case EOnJoinSessionCompleteResult::Type::SessionDoesNotExist:
			CallFailEvent(ESessionFailureReason::JoinSessionFailed, ESessionJoinFailureReason::SessionDoesNotExist);
			break;
		case EOnJoinSessionCompleteResult::Type::CouldNotRetrieveAddress:
			CallFailEvent(ESessionFailureReason::JoinSessionFailed, ESessionJoinFailureReason::CouldNotRetrieveAddress);
			break;
		case EOnJoinSessionCompleteResult::Type::AlreadyInSession:
			CallFailEvent(ESessionFailureReason::JoinSessionFailed, ESessionJoinFailureReason::AlreadyInSession);
			break;
		default:
			CallFailEvent(ESessionFailureReason::JoinSessionFailed, ESessionJoinFailureReason::UnknownError);
		}
		ClearSession(Sessions);

		auto gameInstance = PlayerControllerWeakPtr->GetGameInstance<UDungeonsGameInstance>();
		if (!gameInstance->IsInMenu()) {
			travelutil::OpenMenu(world);
		}
		else {
			gameInstance->EndLoadingScreen(nullptr, "", UDungeonsGameInstance::fDefaultFadeOutTime, UDungeonsGameInstance::fDefaultFadeInTime);
		}
	}
}


void UJoinDungeonsSessionCallbackProxy::OnDestroySessionComplete(FName sessionName, bool bWasSuccessful) {

	auto onlineSystem = online::getCrossplayOss();
	if (!onlineSystem)
	{
		CallFailEvent(ESessionFailureReason::JoinSessionFailed, ESessionJoinFailureReason::UnknownError);
		return;
	}

	auto identity = onlineSystem->GetIdentityIF();
	if (!identity.IsValid()) {
		CallFailEvent(ESessionFailureReason::OnlineSystemNotAvailable, ESessionJoinFailureReason::UnknownError);
		return;
	}

	auto Sessions = online::getSessionInterface();
	if (!Sessions.IsValid()) {
		CallFailEvent(ESessionFailureReason::SessionInterfaceUnavailable, ESessionJoinFailureReason::UnknownError);
		return;
	}

	Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompletionDelegateHandle);
	UWorld* GameWorld = GEngine->GetWorldFromContextObject(WorldContextObject.Get(), EGetWorldErrorMode::ReturnNull);
	if (!GameWorld)
	{
		CallFailEvent(ESessionFailureReason::JoinSessionFailed, ESessionJoinFailureReason::UnknownError);
		return;
	}
	int initialUser = GameWorld->GetGameInstance<UDungeonsGameInstance>()->GetUserManager()->GetInitialUser();
	if (identity->GetLoginStatus(initialUser) != ELoginStatus::LoggedIn) {
		CallFailEvent(ESessionFailureReason::OnlineSystemNotAvailable, ESessionJoinFailureReason::UnknownError);
		return;
	}

	if (bWasSuccessful) {
		OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);
		Sessions->JoinSession(*LocalPlayerId, DungeonsGameSessionName, SearchResult);
	}
	else {
		UE_LOG(LogOnline, Log, TEXT("[JoinDungeonSession] Failed to Destroy session in join session callback"));
	}
}

bool UJoinDungeonsSessionCallbackProxy::ClearAndJoinSession(TSharedPtr<online::Crossplay::Session> Sessions) {
	OnDestroySessionCompletionDelegateHandle = Sessions->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompletionDelegate);
	if (!ClearSession(Sessions)) {
		Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompletionDelegateHandle);
		return false;
	}
	return true;
}

bool UJoinDungeonsSessionCallbackProxy::ClearSession(TSharedPtr<online::Crossplay::Session> Sessions) {
	if (FNamedOnlineSession* existingSession = Sessions->GetNamedSession(DungeonsGameSessionName)) {
		UE_LOG(LogOnline, Log, TEXT("Existing Session In State: %s"), EOnlineSessionState::ToString(existingSession->SessionState));
		return Sessions->DestroySession(DungeonsGameSessionName);
	}
	return false;
}
