#include "Dungeons.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "FindDungeonsSessionCallbackProxy.h"
#include "FindSessionsCallbackProxy.h"
#include "Engine.h"
#include "platform/DungeonsVersion.h"
#include "SessionSettings.h"
#include "OnlineUtil.h"
#include "online/crossplay/Session.h"
#include "DungeonsGameInstance.h"
#include "FindSessionHandler.h"

namespace dungeonsFindSessions {
	const FString lanOnlineServiceName = "Null";

	bool showSession(const FOnlineSessionSearchResult& res, UWorld* world) {
		if (!online::shouldShowSession(res, world)) {
			return false;
		}

		SessionSettings ss(res);
		//PS4 sessions seem to always think they are private games.
		if (ss.IsPrivate()) {
			UE_LOG(LogMultiplayer, Log, TEXT("Private games will not be listed"));
			return false;
		}
		return true;
	}
}

UFindDungeonsSessionCallbackProxy::UFindDungeonsSessionCallbackProxy(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer), OnFindSessionDoneDelegate(FOnFindSessionDoneDelegate::CreateUObject(this, &UFindDungeonsSessionCallbackProxy::OnFindSessionsComplete)) {
}

void UFindDungeonsSessionCallbackProxy::BeginDestroy() {
	Super::BeginDestroy();
	if (const auto* World = GetWorldFromContext()) {
		auto* FindSessionHandler = World->GetGameInstance<UDungeonsGameInstance>()->GetFindSessionHandler();
		FindSessionHandler->RemoveCompleteHandle(OnFindSessionDoneDelegateHandle);
		OnFindSessionDoneDelegateHandle.Reset();
	}
}

UFindDungeonsSessionCallbackProxy* UFindDungeonsSessionCallbackProxy::FindDungeonsSessions(UObject* WorldContextObject, int maximumResult, int timeout) {
	UE_LOG(LogMultiplayer, Verbose, TEXT("UFindDungeonsSessionCallbackProxy::FindDungeonsSessions"));
	UFindDungeonsSessionCallbackProxy* node = NewObject<UFindDungeonsSessionCallbackProxy>();

	check(WorldContextObject);
	node->WorldContextObject = WorldContextObject;
	check(IsInGameThread());

	return node;
}

void UFindDungeonsSessionCallbackProxy::Activate() {
	UE_LOG(LogMultiplayer, Verbose, TEXT("UFindDungeonsSessionCallbackProxy::Activate"));
	if (const auto* World = GetWorldFromContext()) {
		auto* FindSessionHandler = World->GetGameInstance<UDungeonsGameInstance>()->GetFindSessionHandler();
		OnFindSessionDoneDelegateHandle = FindSessionHandler->RequestFindSession(OnFindSessionDoneDelegate);
	}
	else {
		FindSessionsFailed.Broadcast(ESessionFailureReason::FindSessionsFailed);
	}
}

void UFindDungeonsSessionCallbackProxy::OnFindSessionsComplete(bool bWasSuccessful, TSharedPtr<FOnlineSessionSearch> SessionResult) {
	UE_LOG(LogMultiplayer, Verbose, TEXT("UFindDungeonsSessionCallbackProxy::OnFindSessionsComplete"));
	OnFindSessionDoneDelegateHandle.Reset();
	if (!SessionResult.IsValid() || !bWasSuccessful) {
		UE_LOG(LogMultiplayer, Log, TEXT("UFindDungeonsSessionCallbackProxy: Failed to find any sessions (SessionSearch=%d, bWasSuccessful=%d)."), SessionResult.IsValid(), bWasSuccessful);
		FindSessionsFailed.Broadcast(ESessionFailureReason::FindSessionsFailed);
		return;
	}

	UE_LOG(LogMultiplayer, Log, TEXT("UFindDungeonsSessionCallbackProxy: Found %d sessions."), SessionResult->SearchResults.Num());

	TArray<FBlueprintSessionResult> Results;
	for (auto& Result : SessionResult->SearchResults) {
		if (!dungeonsFindSessions::showSession(Result, GetWorldFromContext())) {
			continue;
		}

		FBlueprintSessionResult bResult;
		bResult.OnlineResult = Result;
		Results.Add(bResult);

		UE_LOG(LogMultiplayer, Verbose, TEXT("Server '%s' available, %d ping."), *(Result.Session.OwningUserName), Result.PingInMs);
		UE_LOG(LogMultiplayer, VeryVerbose, TEXT("SessionId='%s', UserIdDebug='%s', UserId='%s', UserName='%s', SessionInfoDebug='%s', SessionInfo='%s'")
			, *(Result.GetSessionIdStr())
			, Result.Session.OwningUserId ? *(Result.Session.OwningUserId->ToDebugString()) : *FString("")
			, Result.Session.OwningUserId ? *(Result.Session.OwningUserId->ToString()) : *FString("")
			, *(Result.Session.OwningUserName)
			, *(Result.Session.SessionInfo->ToDebugString())
			, *(Result.Session.SessionInfo->ToString()));
	}
	FindSessionsCompleted.Broadcast(Results);
}

UWorld* UFindDungeonsSessionCallbackProxy::GetWorldFromContext() {
	if (WorldContextObject.IsValid())
		return GEngine->GetWorldFromContextObject(WorldContextObject.Get(), EGetWorldErrorMode::ReturnNull);
	else
		return nullptr;
}
