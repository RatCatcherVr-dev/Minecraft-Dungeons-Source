#pragma once

#include "Net/UnrealNetwork.h"
#include "GameFramework/GameStateBase.h"
#include "DungeonsGameStateBase.generated.h"

class APlayerState;
class ABasePlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayersCountChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStateAdded, ABasePlayerState*, PlayerState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStateRemoved, ABasePlayerState*, PlayerState);

UCLASS()
class DUNGEONS_API ADungeonsGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

protected:
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;

public:

	virtual void AddPlayerController(ABasePlayerController* playerController);
	virtual void RemovePlayerController(ABasePlayerController* playerController);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	TArray<ABasePlayerState*> GetPlayerStates() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual int GetPlayersCount() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual bool IsLobby() const;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnPlayerStateAdded OnPlayerStateAdded;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnPlayerStateRemoved OnPlayerStateRemoved;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnPlayersCountChanged OnPlayersCountChanged;
};
