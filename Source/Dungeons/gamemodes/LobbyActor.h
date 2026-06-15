#pragma once
#include "GameFramework/Info.h"
#include "game/difficulty/DifficultyStats.h"
#include "game/levels.h"
#include "game/LevelSettings.h"
#include "LobbyActor.generated.h"

class ABasePlayerState;

USTRUCT(BlueprintType)
struct DUNGEONS_API FLevelRequest {
	GENERATED_BODY()
	public:

	UPROPERTY(BlueprintReadOnly)
	int32 PlayerId;

	UPROPERTY(BlueprintReadOnly)
	FString PlayerName;

	UPROPERTY(BlueprintReadOnly)
	FLevelSettings LevelSettings;

	UPROPERTY()
	ABasePlayerState* PlayerState = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRequestingLevelChanged, bool, IsRequesting);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRequestChanged, FLevelRequest, Request);
DECLARE_MULTICAST_DELEGATE(FOnMissionSelectionTimedOut);


UCLASS(BlueprintType)
class ALobbyActor : public AInfo {
	GENERATED_BODY()
public:

	ALobbyActor();

	const FLevelRequest& RequestLevel(int32 byPlayerId, APlayerCharacter* requester, const FLevelSettings&);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsRequestingLevel() const;

	bool TryAccept(int numPlayers);

	void Reject();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const FLevelRequest& GetRequest() const;

	void Tick(float DeltaSecs) override;

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	FOnMissionSelectionTimedOut OnMissionSelectionTimedOut;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	float GetTimeout() const;

	UFUNCTION()
	void OnRep_IsRequestingLevelChanged();

	UFUNCTION()
	void OnRep_RequestChanged();

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnRequestingLevelChanged OnRequestingLevelChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnRequestChanged OnRequestChanged;
private:

	UPROPERTY(ReplicatedUsing = OnRep_IsRequestingLevelChanged)
	bool mIsRequestingLevel = false;

	UPROPERTY(Replicated)
	float mTimeout = 0;

	static const float MISSION_SELECTION_TIMEOUT;
	
	UPROPERTY(ReplicatedUsing = OnRep_RequestChanged)
	FLevelRequest mRequest;

	void SetIsRequestingLevel(bool IsRequesting);	
};
