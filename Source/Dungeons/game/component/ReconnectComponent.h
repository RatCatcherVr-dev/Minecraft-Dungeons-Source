#pragma once

#include "CoreMinimal.h"
#include "FindSessionsCallbackProxy.h"
#include "Components/ActorComponent.h"
#include "online/OnlineCommon.h"
#include "online/sessions/FindSessionHandler.h"
#include "ReconnectComponent.generated.h"

struct ReconnectSaveData;

UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class DUNGEONS_API UReconnectComponent : public UActorComponent
{
GENERATED_BODY()
public:
	UReconnectComponent();
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type reason) override;
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool CanReconnect() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsCrossplaySession() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void Reconnect();

	bool HasGuid() const;

	FString GetGuid() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void ClearReconnect();

private:
	UFUNCTION()
	void OnSaveStateLoaded();
	ReconnectSaveData& GetReconnectSaveData() const;

	bool IsLocal() const;
	bool IsLoggedIn() const;
	bool IsClientInGame() const;
	void UpdateSession();
	
	void PrivilageCheckComplete(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults);
	void OnFindSessionsComplete(bool bWasSuccessful, TSharedPtr<FOnlineSessionSearch> SessionResult);

	class UDungeonsGameInstance* Instance = nullptr;
	
	float TimeSinceLastCheck = 0;
	float TimeSinceLastResponse = 100000;
	int ResponseCount = 0;
	int FailedResponseCount = 0;

	TOptional<FBlueprintSessionResult> SessionSearchResult;
	bool ClientCanReconnect = false;

	UPROPERTY()
	class UJoinDungeonsSessionCallbackProxy* JoinSessionProxy;

	FOnFindSessionDoneDelegate OnFindSessionDoneDelegate;
	FDelegateHandle OnFindSessionDoneDelegateHandle;
};
