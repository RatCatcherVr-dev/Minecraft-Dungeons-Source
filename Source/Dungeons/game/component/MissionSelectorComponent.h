#pragma once

#include "Components/ActorComponent.h"
#include "gamemodes/LobbyActor.h"
#include "DungeonsGameInstance.h"
#include "game/mission/request/MissionRequest.h"
#include "MissionSelectorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLevelRequested, const FLevelRequest&, Request, bool, isOwnerOfRequest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevelStarting);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevelRequestFinished);


class ALobbyActor;
class ABasePlayerController;

UCLASS(BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), DisplayName = "MissionSelectorComponent")
class DUNGEONS_API UMissionSelectorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void BeginPlay() override;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnLevelRequested OnLevelRequested;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnLevelRequestFinished OnLevelRequestFinished;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnLevelStarting OnLevelStarting;

	using MissionRequestProvider = std::function<TOptional<FMissionRequest>()>;
	bool TryStartMission(APlayerCharacter* RequestingPlayer, const MissionRequestProvider& requestProvider);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool TryStartMissionSelection(APlayerCharacter* RequestingPlayer, const FMissionSelection& missionSelection);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool TryContinueMission(APlayerCharacter* RequestingPlayer, ELevelNames levelName);
	
	//Sent from client to server
	UFUNCTION(Server, Reliable, WithValidation, Category = "Dungeons")
	void ServerRequestLevel(APlayerCharacter* requesterPlayer, const FMissionRequest& missionRequest);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Dungeons")
	void ServerAccept();

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Dungeons")
	void ServerReject();

	UFUNCTION()
	void HandleLogout();

	UFUNCTION()
	void TravelToMission();

private:

	void SelectionTimedOut();

	UPROPERTY()
	ALobbyActor* mLobbyActor = nullptr;

	bool _ServerInternalAccept();

	void InternalReject();

	bool IsInLobby();

	TArray<UMissionSelectorComponent*> GetMissionSelectors() const;

	int32 GetNumPlayers();

	bool CanRequest() const;

	FTimerHandle TravelTimerHandle;
	
	//Broadcasting from server to other clients
	UFUNCTION(Client, Reliable, Category = "Dungeons")
	void ClientLevelRequested(const FLevelRequest& Request);

	//Broadcasting from server to other clients just as it is starting
	UFUNCTION(Client, Reliable, Category = "Dungeons")
	void ClientLevelStarting(FLevelSettings levelSettings);

	UFUNCTION(Client, Reliable, Category = "Dungeons")
	void ClientRequestFinished();

	UDungeonsGameInstance* GetGameInstance() const;
};
