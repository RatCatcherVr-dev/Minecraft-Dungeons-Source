#pragma once

#include "CoreMinimal.h"
#include "DungeonsSessionCommon.h"
#include "OnlineBlueprintCallProxyBase.h"
#include "Dungeons/online/OnlineCommon.h"
#include "SessionSettings.h"
#include "CreateDungeonsSessionCallbackProxy.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FCreateSessionFailed, ESessionFailureReason);
DECLARE_MULTICAST_DELEGATE_OneParam(FCreateSessionSucceeded, const FLevelSettings&);

class APlayerController;

UCLASS()
class DUNGEONS_API UCreateDungeonsSessionCallbackProxy : public UObject {
	GENERATED_UCLASS_BODY()
public:

	FCreateSessionSucceeded CreateSessionSucceeded;
	FCreateSessionFailed CreateSessionFailed;

	static UCreateDungeonsSessionCallbackProxy* CreateLobbySession(const UObject* WorldContextObject, bool isPrivate);

	static UCreateDungeonsSessionCallbackProxy* CreateSession(const UObject* WorldContextObject, const FLevelSettings&, bool isPrivate);

	void setLanMatch(bool isLan);

	void Activate();

private:
	const UObject* WorldContextObject;
	SessionSettings Settings;
	TSharedPtr<const FUniqueNetId> LocalPlayerId;
	FLevelSettings LevelSettings;

	void OnCreateSessionComplete(FName sessionName, bool bWasSuccessful);
	void OnStartOnlineGameComplete(FName sessionName, bool bWasSuccessful);
	void OnDestroySessionComplete(FName sessionName, bool bWasSuccessful);
	void OnSessionFailure(const FUniqueNetId& uniqueNetId, ESessionFailure::Type type);

	void FailInternal(ESessionFailureReason);

	FOnDestroySessionCompleteDelegate OnDestroySessionCompletionDelegate;
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
	FOnSessionFailureDelegate OnSessionFailureDelegate;

	FDelegateHandle OnDestroySessionCompletionDelegateHandle;
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;
	FDelegateHandle OnSessionFailureDelegateHandle;

	ESessionFailureReason TemporaryReason = ESessionFailureReason::Unknown;
};
