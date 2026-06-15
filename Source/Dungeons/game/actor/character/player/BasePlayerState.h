#pragma once

#include "GameFramework/PlayerState.h"
#include "GameFramework/SaveGame.h"
#include "GameFramework/PlayerController.h"
#include "Dungeons/platform/GameVersion.h"
#include "BasePlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerName, class ABasePlayerState*, playerState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerIsHostChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerPlatformChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReadyChanged);
DECLARE_MULTICAST_DELEGATE(FOnPlayerNumberChangedInternal)

UCLASS()
class DUNGEONS_API ABasePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ABasePlayerState();

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	void ClientInitialize(AController* C) override;

	void BeginPlay() override;

	void SetOwner(AActor* netOwner) override;

	UFUNCTION()
	void OnRep_OwnedByHost();

	UFUNCTION()
	void OnRep_SetReady();

	UFUNCTION()
	void OnRep_PlayerNumber();

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnPlayerName OnPlayerNameChanged;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnPlayerIsHostChanged OnPlayerIsHostChanged;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnPlayerPlatformChanged OnPlayerPlatformChanged;

	FOnPlayerNumberChangedInternal OnPlayerNumberChanged;
	
	UFUNCTION(BlueprintCallable)
	void SetRespawnSeconds(float seconds);
	
	UFUNCTION(BlueprintCallable)
	float GetRespawnSeconds() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsReady() const;

	UFUNCTION(Server, WithValidation, Reliable)
	void SetReady(bool ready);

	UFUNCTION(Server, WithValidation, Reliable)
	void SetOwnedByHost(bool owned);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_OwnedByHost, Replicated)
	bool OwnedByHost = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_SetReady, Replicated)
	bool mReady = false;

	
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnReadyChanged OnReadyChanged;

	bool IsDisplayNameAssigned() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FString GetPlayerDisplayName() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FString GetPlayerPrimaryDisplayName() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FString GetPlayerSecondaryDisplayName() const;

	void SetPlayerNumber(int number);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	int GetPlayerNumber() const;

	void SetPlayerPlatform(EPlatformType);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	EPlatformType GetPlayerPlatform() const;

	void RefreshDisplayName();

	void RefreshSecondaryUniqueId();

	void RefreshPlayerPlatform();

	void SetSecondaryUniqueId(const FUniqueNetIdRepl& InUniqueId);

	void RegisterPlayerWithSession(bool bWasFromInvite) override;

	void UnregisterPlayerWithSession() override;

	UPROPERTY(BlueprintReadWrite, Replicated)
	bool bLocallyDroppingIn = false;

	UPROPERTY(Replicated)
	FUniqueNetIdRepl SecondaryUniqueId; 
private:

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerAssignSecondaryUniqueId(const FUniqueNetIdRepl& InUniqueId);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerAssignDisplayName(const FString& primaryName, const FString& secondaryName);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerAssignPlayerPlatform(EPlatformType platform);

	UPROPERTY(Replicated)
	bool mDisplayNameAssigned = false;

	UFUNCTION()
	void OnRep_DisplayName();

	UFUNCTION()
	void OnRep_SecondaryDisplayName();

	UFUNCTION()
	void OnRep_PlayerPlatform();

	UPROPERTY(ReplicatedUsing = OnRep_DisplayName)
	FString mDisplayName;

	UPROPERTY(ReplicatedUsing = OnRep_SecondaryDisplayName)
	FString mSecondaryDisplayName;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_PlayerPlatform)
	EPlatformType PlayerPlatform;

	void SetPlayerDisplayName(const FString& primaryName, const FString& secondaryName);

protected:

	void CopyProperties(APlayerState* PlayerState) override;

	UPROPERTY(BlueprintReadOnly, Replicated)
	float RespawnSeconds = 5.f;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_PlayerNumber)
	int PlayerNumber = 0;
};
