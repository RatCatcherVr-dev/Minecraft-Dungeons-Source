#include "Dungeons.h"
#include "CreateDungeonsSessionCallbackProxy.h"
#include "Engine.h"
#include "DungeonsGameInstance.h"
#include "SessionSettings.h"
#include "OnlineUtil.h"
#include "DungeonsUserManagement.h"

#if PLATFORM_SWITCH
#define REQURES_LOGIN 1
#else
#define REQURES_LOGIN 0
#endif


UCreateDungeonsSessionCallbackProxy::UCreateDungeonsSessionCallbackProxy(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
	, WorldContextObject(nullptr)
	, OnCreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &UCreateDungeonsSessionCallbackProxy::OnCreateSessionComplete))
	, OnStartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &UCreateDungeonsSessionCallbackProxy::OnStartOnlineGameComplete))
	, OnDestroySessionCompletionDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &UCreateDungeonsSessionCallbackProxy::OnDestroySessionComplete))
	, OnSessionFailureDelegate(FOnSessionFailureDelegate::CreateUObject(this, &UCreateDungeonsSessionCallbackProxy::OnSessionFailure))
{
}

UCreateDungeonsSessionCallbackProxy* UCreateDungeonsSessionCallbackProxy::CreateLobbySession(const UObject* WorldContextObject, bool isPrivate) {
	return CreateSession(WorldContextObject, levelsettingsutil::generateLobbySettings(WorldContextObject->GetWorld()), isPrivate);
}

UCreateDungeonsSessionCallbackProxy* UCreateDungeonsSessionCallbackProxy::CreateSession(const UObject* WorldContextObject, const FLevelSettings& LevelSettings, bool isPrivate) {
	UCreateDungeonsSessionCallbackProxy* node = NewObject<UCreateDungeonsSessionCallbackProxy>();
	node->WorldContextObject = WorldContextObject;	
	node->Settings = online::createSessionSettings(WorldContextObject->GetWorld(), LevelSettings, isPrivate, online::IsCrossplayEnabled());
	node->LevelSettings = LevelSettings;
	return node;
}

void UCreateDungeonsSessionCallbackProxy::setLanMatch(bool isLan) {
	Settings.SetLanMatch(isLan);
}

void UCreateDungeonsSessionCallbackProxy::Activate() {
	auto identity = online::getIdentityInterface();
	if (!identity.IsValid()) {
		UE_LOG(LogOnline, Log, TEXT("UCreateDungeonsSessionCallbackProxy::Activate(): could not find online system indentity interface"));
		FailInternal(ESessionFailureReason::OnlineSystemNotAvailable);
		return;
	}

	UWorld* GameWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	int initialUser = GameWorld->GetGameInstance<UDungeonsGameInstance>()->GetUserManager()->GetInitialUser();
	LocalPlayerId = identity->GetUniquePlayerIdForSessionSubsystem(initialUser);
	if (identity->GetLoginStatus(initialUser) != ELoginStatus::LoggedIn) {
		UE_LOG(LogOnline, Log, TEXT("UCreateDungeonsSessionCallbackProxy::Activate(): Local user is not logged in"));
		FailInternal(ESessionFailureReason::LoginFailed);
		return;
	}

	const auto& OnlineSession = online::getSessionInterface();
	if (!OnlineSession.IsValid()) {
		check(!"The session interface should always exist at this point.");
		FailInternal(ESessionFailureReason::OnlineSystemNotAvailable);
		return;
	}

	check(!OnCreateSessionCompleteDelegateHandle.IsValid() && "This might mean we're trying to create two sessions simultaneous.");
	OnlineSession->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);

	OnCreateSessionCompleteDelegateHandle = OnlineSession->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

	if (FNamedOnlineSession* existingSession = OnlineSession->GetNamedSession(DungeonsGameSessionName)) {
		UE_LOG(LogOnline, Error, TEXT("Existing Session In State: %s"), EOnlineSessionState::ToString(existingSession->SessionState));
		check(!"The session should have been destroyed earlier.");

		FailInternal(ESessionFailureReason::CreateSessionFailed);
	}
	else {
		if (!OnlineSession->CreateSession(initialUser, DungeonsGameSessionName, Settings.Get())) {
			UE_LOG(LogOnline, Log, TEXT("UCreateDungeonsSessionCallbackProxy::Activate(): Failed to create new session."));
			FailInternal(ESessionFailureReason::CreateSessionFailed);
		}
	}
}

void UCreateDungeonsSessionCallbackProxy::OnCreateSessionComplete(FName sessionName, bool bWasSuccessful) {
	UWorld* GameWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	const auto& OnlineSession = online::getSessionInterface();
	OnlineSession->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);

	if (!bWasSuccessful) {
		FailInternal(ESessionFailureReason::CreateSessionFailed);
		return;
	}

	OnStartSessionCompleteDelegateHandle = OnlineSession->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);
	OnlineSession->StartSession(sessionName);
}

void UCreateDungeonsSessionCallbackProxy::OnStartOnlineGameComplete(FName sessionName, bool bWasSuccessful) {                                                                          
	UWorld* GameWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	const auto& OnlineSession = online::getSessionInterface();
	OnlineSession->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);

	if (!bWasSuccessful) {
		FailInternal(ESessionFailureReason::CreateSessionFailed);
		return;
	}

	FNamedOnlineSession* session = OnlineSession->GetNamedSession(sessionName);
	if (!session) {
		check(!"How can the session start successfully but still not exist?");
		FailInternal(ESessionFailureReason::CreateSessionFailed);
		return;
	}

	CreateSessionSucceeded.Broadcast(LevelSettings);

	if (LocalPlayerId->IsValid()) {
		OnlineSession->RegisterPlayer(sessionName, *LocalPlayerId, false);
	}
}

void UCreateDungeonsSessionCallbackProxy::OnSessionFailure(const FUniqueNetId& uniqueNetId, ESessionFailure::Type type) {
	UE_LOG(LogOnline, Log, TEXT("[Session] OnSessionFailure"));
}

void UCreateDungeonsSessionCallbackProxy::OnDestroySessionComplete(FName sessionName, bool bWasSuccessful) {
	UWorld* GameWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	online::getSessionInterface()->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompletionDelegateHandle);

	check(bWasSuccessful && "We have no good fallback here.");
	CreateSessionFailed.Broadcast(TemporaryReason);
}

void UCreateDungeonsSessionCallbackProxy::FailInternal(ESessionFailureReason reason) {
	UWorld* GameWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	const auto& OnlineSession = online::getSessionInterface();
	if (FNamedOnlineSession* existingSession = OnlineSession->GetNamedSession(DungeonsGameSessionName)) {
		// Need to clean up the mess
		TemporaryReason = reason;

		OnDestroySessionCompletionDelegateHandle = OnlineSession->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompletionDelegate);
		if (OnlineSession->DestroySession(DungeonsGameSessionName)) {
			return;
		}
		else {
			OnlineSession->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompletionDelegateHandle);
			check(!"We couldn't destroy the session that was created during the failed attempt to create a session.")
		}
	}

	CreateSessionFailed.Broadcast(reason);
}
