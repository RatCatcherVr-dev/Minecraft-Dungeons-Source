#pragma once

#include "online/sessions/CreateDungeonsSessionCallbackProxy.h"
#include "PlayerControllerBase.h"
#include "BaseMenuPlayerController.generated.h"

namespace online
{
namespace Crossplay
{
class Session;
}
}

UCLASS()
class DUNGEONS_API ABaseMenuPlayerController : public APlayerControllerBase {
	GENERATED_BODY()
public:
	ABaseMenuPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	void StartOnlineGame(bool IsPrivate);

	UFUNCTION(BlueprintCallable)
	void StartOfflineGame();


	//D11.KS - Need for a late call
	UFUNCTION(BlueprintCallable)
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// D11.DB
	void OnGamepadActiveChanged(bool GamepadActive) override;

	virtual void GetSeamlessTravelActorList(bool bToEntry, TArray<class AActor*>& ActorList) override;

private:
	void StartGame(bool HostGame, bool IsPrivate);
	void OnPrivilegesTaskFinished(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults);
	void OnHasPrivileges();

	bool ClearSession(TSharedPtr<online::Crossplay::Session> Sessions);
	void OnDestroySessionComplete(FName sessionName, bool bWasSuccessful);
	
	UCreateDungeonsSessionCallbackProxy* CreateSessionProxy;

	FOnDestroySessionCompleteDelegate OnDestroySessionCompletionDelegate;
	FDelegateHandle OnDestroySessionCompletionDelegateHandle;

	uint8 CurrentMapLoadType;
	FLevelSettings CurrentLevelSettings;
};
