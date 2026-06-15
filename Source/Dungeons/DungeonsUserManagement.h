#pragma once
#include "game/actor/character/player/BaseMenuPlayerController.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/actor/character/player/BaseMenuPlayerController.h"
#include "DelegateCombinations.h"
#include "DungeonsLoginFlow.h"
#include "DungeonsUserManagement.generated.h"

UENUM(BlueprintType)
enum class ELocalUserLoginResult : uint8
{
	Success,
	UserAlreadyInGame,
	Error,
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUserManagementUserSignedOut, int, LocalPlayerIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUserManagementUserAdded, int, LocalPlayerIndex, APlayerController*, PlayerController);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoginRequest, int, ControllerID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoginRequestComplete, int, ControllerIndex, ELocalUserLoginResult, Result);

DECLARE_MULTICAST_DELEGATE(FOnAddedLocalPlayer);
DECLARE_MULTICAST_DELEGATE(FOnRemovedLocalPlayer);


class UDungeonsGameInstance;

UCLASS()
class UDungeonsUserManager : public UObject
{
	GENERATED_BODY()
public:
	UDungeonsUserManager(const FObjectInitializer& ObjectInitializer);


	static UDungeonsUserManager* spInstance;
	static UDungeonsUserManager* Instance();

	UPROPERTY(BlueprintAssignable, Category = "UserManagement")
	FUserManagementUserSignedOut OnUserSignedOutDelegate;

	UPROPERTY(BlueprintAssignable, Category = "UserManagement")
	FUserManagementUserAdded OnUserAddedDelegate;

	FOnAddedLocalPlayer OnAddedLocalPlayer;
	FOnRemovedLocalPlayer OnRemovedLocalPlayer;


	UPROPERTY(BlueprintAssignable, Category = "UserManagement")
	FOnLoginRequest OnLoginRequest;
	UPROPERTY(BlueprintAssignable, Category = "UserManagement")
	FOnLoginRequestComplete OnLoginRequestComplete;

	UFUNCTION()
	void OnLoginRequestEnded(int LocalPlayerIndex, APlayerController* PlayerController, ELoginResult Result);

	UFUNCTION(BlueprintCallable, Category = "UserManagement")
	void TryPlayerControllerPlatformLogin(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "UserManagement")
	void Print(FString str);

	// D11.DJB - Intended for use with PS4 only.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UserManagement")
	int32 GetServiceInitialUserLocalIndex();

	UFUNCTION(BlueprintCallable, Category = "UserManagement")
	APlayerController* CreatePlayer(int32 ControllerId, bool bSpawnPawn);

	UFUNCTION(BlueprintCallable, Category = "UserManagement")
	bool RemovePlayer(int32 ControllerId);

	UFUNCTION(BlueprintCallable, Category = "UserManagement")
	void RemoveExtraLocalPlayers();

	UFUNCTION(BlueprintCallable, Category = "UserManagement")
	int GetLocalPlayerIndexFromPlayerController(APlayerController* controller);

	UFUNCTION(BlueprintCallable, Category = "UserManagement")
	int GetLocalPlayerIndex(ULocalPlayer* localPlayer);

	// #D11.CM
	UFUNCTION(BlueprintCallable, Category = "UserManagement")
	TArray<ULocalPlayer*> GetLocalPlayers();

	UFUNCTION(BlueprintCallable, Category = "UserManagement")
	TArray<APlayerControllerBase*> GetAllLocalPlayerControllers();

	// #D11.CM
	UFUNCTION(BlueprintCallable, Category = "UserManagement", meta = (DeprecatedFunction, DeprecationMessage = "Please use the GetAllLocalPlayerControllers instead."))
	TArray<ABasePlayerController*> GetLocalPlayerControllers();

	UFUNCTION(BlueprintCallable, Category = "UserManagement", meta = (DeprecatedFunction, DeprecationMessage = "Please use the GetAllLocalPlayerControllers instead."))
	TArray<ABaseMenuPlayerController*> GetLocalMenuPlayerControllers();

	UFUNCTION(BlueprintCallable, Category = "UserManagement")
	APlayerController* GetInitialPlayerController() const;

	ULocalPlayer* GetInitialLocalPlayer() const;

	UFUNCTION(BlueprintCallable, Category = "UserManagement")
	void Logout(int32 localUserNum);

	UFUNCTION(BlueprintCallable, Category = "UserManagement")
	int32 GetInitialUser();

	UFUNCTION(BlueprintCallable, Category = "UserManagement")
	int32 GetInitialUserSystemId();

	UFUNCTION(BlueprintCallable, Category = "UserManagement")
	APlayerController* GetPlayerControllerFromControllerID(int ControllerId);

	UFUNCTION(BlueprintCallable, Category = "UserManagement")
	FString GetLocalUserName(int ControllerId);

	UFUNCTION(BlueprintCallable, Category = "UserManagement")
	FString GetSwitchOfflineUserName(int ControllerId);

	UFUNCTION(BlueprintCallable, Category = "UserManagement")
	int GetLocalUserNumFromControllerID(int ControllerId);

	UFUNCTION(BlueprintCallable, Category = "UserManagement")
	int GetLocalUserSystemIdFromControllerID(int ControllerId);

	void InitialUserSignedOut();

private:

	void AddUser(int LocalPlayerNum, APlayerController* PlayerController);

	UDungeonsGameInstance *GetGameInstance();

	void ResetLocalPlayers();
};
