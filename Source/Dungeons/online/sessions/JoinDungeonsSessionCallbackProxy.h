#pragma once

#include "CoreMinimal.h"
#include "DungeonsSessionCommon.h"
#include "FindSessionsCallbackProxy.h"
#include "OnlineBlueprintCallProxyBase.h"
#include "Dungeons/online/OnlineCommon.h"
#include "JoinDungeonsSessionCallbackProxy.generated.h"

struct FBlueprintSessionResult;
class APlayerController;
namespace online
{
namespace Crossplay
{
class Session;
}
}

UENUM(BlueprintType)
enum class ESessionJoinFailureReason : uint8 {
	SessionIsFull,
	SessionDoesNotExist,
	CouldNotRetrieveAddress,
	AlreadyInSession,
	UnknownError
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FJoinSessionFailed, ESessionFailureReason, Reason, ESessionJoinFailureReason, ExtendedReason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FJoinSessionCompleted);

UCLASS()
class DUNGEONS_API UJoinDungeonsSessionCallbackProxy : public UOnlineBlueprintCallProxyBase {
	GENERATED_UCLASS_BODY()
public:

	UPROPERTY(BlueprintAssignable)
	FJoinSessionCompleted JoinSessionCompleted;

	UPROPERTY(BlueprintAssignable)
	FJoinSessionFailed JoinSessionFailed;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Dungeons|Online")
	static UJoinDungeonsSessionCallbackProxy* JoinDungeonsSession(UObject* WorldContextObject, class APlayerController* PlayerController, const FBlueprintSessionResult& SearchResult);

	static UJoinDungeonsSessionCallbackProxy* JoinDungeonsSession(UObject* WorldContextObject, class APlayerController* PlayerController, const FOnlineSessionSearchResult& SearchResult);
	
	virtual void Activate() override;

private:

	void OnCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName sessionName, bool bWasSuccessful);
	bool ClearAndJoinSession(TSharedPtr<online::Crossplay::Session> Sessions);
	bool ClearSession(TSharedPtr<online::Crossplay::Session> Sessions);

	void CallFailEvent(ESessionFailureReason Reason, ESessionJoinFailureReason ExtendedReason);

	FOnDestroySessionCompleteDelegate OnDestroySessionCompletionDelegate;
	FDelegateHandle OnDestroySessionCompletionDelegateHandle;

	TWeakObjectPtr<UObject> WorldContextObject;
	TWeakObjectPtr<APlayerController> PlayerControllerWeakPtr;
	TSharedPtr<const FUniqueNetId> LocalPlayerId;
	FOnlineSessionSearchResult SearchResult;

	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
};
