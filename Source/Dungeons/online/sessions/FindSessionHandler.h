#pragma once

#include "CoreMinimal.h"
#include "ObjectMacros.h"
#include "OnlineDelegateMacros.h"
#include "OnlineSessionInterface.h"

#include "FindSessionHandler.generated.h"

class ABasePlayerController;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnFindSessionDone, bool, TSharedPtr<FOnlineSessionSearch>);
typedef FOnFindSessionDone::FDelegate FOnFindSessionDoneDelegate;

UCLASS()
class DUNGEONS_API UFindSessionHandler : public UObject {
	GENERATED_BODY()

public:
	UFindSessionHandler();
	~UFindSessionHandler();

	FDelegateHandle RequestFindSession(const FOnFindSessionDoneDelegate&);
	void RemoveCompleteHandle(FDelegateHandle);

private:

	bool Tick(float delay);
	bool ShouldDoFindSession() const;
	void DoBroadcast(bool bWasSuccessful);
	void DoFindSession();
	void OnFindSessionsComplete(bool bWasSuccessful);
	const ABasePlayerController* GetPlayerController();
	const FUniqueNetId* GetUniqueNetId();

	FDelegateHandle TickTimerHandle;
	FTickerDelegate TickDelegate;
	FOnFindSessionDone OnFindSessionDone;
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;

	bool ShouldFindSession;
	bool FindSessionIsActive;
	TSharedPtr<class FOnlineSessionSearch> CurrentSessionSearch;
};
