#pragma once
#include "CoreMinimal.h"
#include "game/actor/character/player/BaseMenuPlayerController.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/actor/character/player/BaseMenuPlayerController.h"
#include "online/OnlineFunctionTypes.h"
#include "DelegateCombinations.h"
#include "DungeonsLoginFlow.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FLoginComplete , int, LocalPlayerIndex, APlayerController*, PlayerController, ELoginResult, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShowAccountLinkScreen, const FString&, url, const FString&, code);
/* action to call once the login is complete */
DECLARE_DELEGATE(FPostLoginAction);

class UDungeonsGameInstance;
class UDungeonsUserManager;

UCLASS()
class UDungeonsLoginFlow : public UObject
{
	GENERATED_BODY()
public:
	UDungeonsLoginFlow(const FObjectInitializer& ObjectInitializer);

	void BeginDestroy() override;

	UFUNCTION(BlueprintCallable, Category = "Dungeons|LoginFlow")
	void Login(int ControllerId, APlayerController* InPlayerController);

	virtual void LocalPlayerLogin(int ControllerId, APlayerController* PlayerController);
	
	bool IsPlayerLoggingIn()const { return LoggingInPlayerController.Get() != nullptr; }

	APlayerController *GetLoggingInPlayerController() { return LoggingInPlayerController.Get(); }

	UFUNCTION()
	void OnLoadSaveDataCompleted();

	UFUNCTION(BlueprintCallable, Category = "Dungeons|LoginFlow")
	void DoPostLoginAction();

	UPROPERTY(BlueprintAssignable, Category = "Dungeons|LoginFlow")
	FLoginComplete OnLoginComplete;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons|LoginFlow")
	FOnShowAccountLinkScreen OnShowAccountLinkScreen;

#if !UE_BUILD_SHIPPING && (PLATFORM_PS4 || PLATFORM_SWITCH)
	void SetDebugLoginResult(ELoginResult);
	bool DebugLoginResultSet();
	void ClearDebugLoginResult();
#endif

	void RefreshLogin() const;
protected:
	void EnableLoginRefresh();
	void DisableLoginRefresh();

	FTimerHandle PeriodicLoginRefreshHandle;
	FDelegateHandle ResumptionLoginRefreshHandle;

	virtual void PlatformLogin(ULocalPlayer* LocalPlayer);
	void OnLoginCompleted(TSharedPtr<const FUniqueNetId> UniqueId, int LocalPlayerIndex, const FOnlineError& LoginResult);

	ELoginResult SetLoginUser(TSharedPtr<const FUniqueNetId> UniqueId, int LocalPlayerNum);
	void SetPlayerControllerId(int ControllerId);
	int GetPlayerControllerId();

	void LogoutOfOss(TSharedPtr<const FUniqueNetId> UniqueId);

	UDungeonsGameInstance *GetGameInstance() const;
	void LoadSaveData(int LocalPlayerIndex);

	void BroadcastLoginComplete(ELoginResult Result);

#if !UE_BUILD_SHIPPING && (PLATFORM_PS4 || PLATFORM_SWITCH)
	ELoginResult debugLoginResult = ELoginResult::Success;
#endif

	FPostLoginAction PostLoginAction;
	// The player controller triggering things
	TWeakObjectPtr<APlayerController> LoggingInPlayerController;
	static constexpr float AUTHENTICATION_REFRESH_PERIOD_SECONDS = 10.f * 60.f;

	std::function<void()> GetLoginRefreshFunction() const;
};
