#pragma once

#include "CoreMinimal.h"
#include "DungeonsSessionCommon.h"
#include "FindSessionsCallbackProxy.h"
#include "online/crossplay/CrossplayOSS.h"
#include "UObject/ObjectMacros.h"
#include "FindSessionHandler.h"
#include "CrossplaySessionResult.h"

#include "FindDungeonsSessionCallbackProxy.generated.h"

class ABasePlayerController;
class APlayerController;
class FUniqueNetId;
class IOnlineSession;
class IOnlineSubsystem;
class UWorld;


namespace online
{
namespace Crossplay
{
class Session;
}
}

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFindSessionsFailed, ESessionFailureReason, Reason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFindSessionsCompleted, const TArray<FBlueprintSessionResult>&, SearchResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFindSessionOngoing);

UCLASS()
class DUNGEONS_API UFindDungeonsSessionCallbackProxy : public UOnlineBlueprintCallProxyBase {
	GENERATED_UCLASS_BODY()
public:

	UPROPERTY(BlueprintAssignable)
	FFindSessionsCompleted FindSessionsCompleted;

	UPROPERTY(BlueprintAssignable)
	FFindSessionsFailed FindSessionsFailed;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Dungeons|Online")
	static UFindDungeonsSessionCallbackProxy* FindDungeonsSessions(UObject* WorldContextObject, int maximumResult, int timeout);

	void Activate() override;
	void BeginDestroy() override;

private:
	void OnFindSessionsComplete(bool, TSharedPtr<FOnlineSessionSearch>);
	UWorld* GetWorldFromContext();

	TWeakObjectPtr<UObject> WorldContextObject;

	FOnFindSessionDoneDelegate OnFindSessionDoneDelegate;
	FDelegateHandle OnFindSessionDoneDelegateHandle;
};
