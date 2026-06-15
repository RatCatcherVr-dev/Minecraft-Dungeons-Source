#pragma once

#include "Net/UnrealNetwork.h"
#include "gamemodes/DungeonsGameStateBase.h"
#include "game/component/AwardsGeneratorComponent.h"
#include "game/component/MissionProgressComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/player/BasePlayerState.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/difficulty/DifficultyStats.h"
#include "game/cinematic/CinematicSequence.h"
#include "DungeonsGameState.generated.h"

class UMaterialParameterCollectionInstance;
class UMaterialParameterCollection;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIsGameOver, bool, isGameOver);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAnnouncePlayerDown, APlayerCharacter*, player);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReviveEnabledChanged, bool, ReviveEnabled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShowLivesChanged);

//Cinematics - Level intros
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCinematicSequenceChanged, UCinematicSequence*, newCinematic);

//Lives
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLivesChanged);
DECLARE_MULTICAST_DELEGATE(FOnLivesLostThisSessionChanged);

//Team Power
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAverageTeamPowerChanged);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPostSeamlessTravel, const char*);
DECLARE_MULTICAST_DELEGATE(FOnPreSeamlessTravel);

DECLARE_MULTICAST_DELEGATE(FOnRevivePossibleChanged);
DECLARE_MULTICAST_DELEGATE(FOnPlayerRemoved);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerRemoving, APlayerCharacter*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerAdding, APlayerCharacter*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerAddedOrRemoved);
DECLARE_MULTICAST_DELEGATE(FOnPlayerAddedOrRemovedInternal);

DECLARE_MULTICAST_DELEGATE(FOnPartsDiscoveredChanged);

UCLASS()
class DUNGEONS_API ADungeonsGameState : public ADungeonsGameStateBase
{
	GENERATED_BODY()

public:
	ADungeonsGameState();

	void AddPlayerCharacter(APlayerCharacter* playerCharacter);
	void RemovePlayerCharacter(APlayerCharacter* playerCharacter);

	void OnConstruction(const FTransform &transform) override;
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly, Replicated)
	int DevelopmentLevelSeed = 0;

	void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void SetIsGameOver(bool gameover);
	
	UFUNCTION(BlueprintCallable)
	int GetPlayersMatchingAliveStateCount(EAliveState aliveState) const;

	UFUNCTION(BlueprintCallable)
	bool IsAnyPlayerMatchingAliveState(EAliveState aliveState) const;
	
	FOnPostSeamlessTravel OnPostSeamlessTravel;
	FOnPreSeamlessTravel OnPreSeamlessTravel;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons|Cinematic")
	FOnCinematicSequenceChanged OnCinematicSequenceChanged;

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Cinematic")
	UCinematicSequence* GetCinematic() const;

	bool IsCinematicPlaying() const;

	bool IsCinematicLoading() const;
	bool HasCinematicStartedLoading() const;
	
	void StartLoadingCinematic(FSoftObjectPath ClassPath, Placement placement, BlockPos originalPos);

	bool PlayLoadedCinematic();
	
	void OnCinematicCannotPlay();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	int32 GetLives() const;

	void SetLives(int newLivesLeft);

	void SubtractLives(int livesToSubtract = 1);

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnLivesChanged OnLivesChanged;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool GetShowLives() const;

	void SetShowLives(bool areLivesLimited);

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnShowLivesChanged OnShowLivesChanged;

	int GetLivesLostThisSession() const;
	FOnLivesLostThisSessionChanged OnLivesLostThisSessionChanged;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnAnnouncePlayerDown OnAnnouncePlayerDown;

	FOnPlayerRemoved OnPlayerRemoved;
	FOnPlayerRemoving OnPlayerRemoving;
	FOnPlayerAdding OnPlayerAdding;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnPlayerAddedOrRemoved OnPlayerAddedOrRemoved;
	FOnPlayerAddedOrRemovedInternal OnPlayerAddedOrRemovedInternal;



	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	float GetAverageTeamPower() const;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnAverageTeamPowerChanged OnAverageTeamPowerChanged;



	void SetReviveEnabled(bool Possible);

	FOnRevivePossibleChanged OnRevivePossibleChanged;
		
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnReviveEnabledChanged OnReviveEnabledChanged;

	bool CanAnyoneReviveMe(APlayerCharacter* player) const;

	void PlayerAliveStateChanged(APlayerCharacter* player) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool GetReviveEnabled() const;

	bool IsMultiplayer() const;

	bool IsPendingRemoval(APlayerCharacter* player) const;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnIsGameOver OnIsGameOver;

	int  GetPartsDiscovered() const;
	void SetPartsDiscovered(int);
	FOnPartsDiscoveredChanged OnPartsDiscoveredChanged;
protected:	
	void Tick(float deltaSeconds) override;

	void StartCinematic(UCinematicSequence* cinematic);
	void StartLoadCinematicInstance(UCinematicSequence* cinematic);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool GetIsGameOver() const;

	UFUNCTION()
	void OnRep_OnIsGameOver();

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_OnIsGameOver)
	bool IsGameOver;
	
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Dungeons")
	TWeakObjectPtr<UCinematicSequence> mCinematic;

	UPROPERTY()
	UCinematicSequence* mLoadingCinematic;

	UPROPERTY()
	TWeakObjectPtr<UMaterialParameterCollectionInstance> materialParameterCollectionInstance;

	UPROPERTY()
	UMaterialParameterCollection* MaterialParameterCollection;

private:
	void UpdatePlayersPendingRemoval();	

	bool IsPlayerInWorld(APlayerCharacter* player) const;

	UFUNCTION()
	void OnRep_Lives();

	UPROPERTY(ReplicatedUsing = OnRep_Lives)
	int32 Lives;

	UFUNCTION()
	void OnRep_ShowLives();

	UPROPERTY(ReplicatedUsing = OnRep_ShowLives)
	bool ShowLives = false;

	UFUNCTION()
	void OnRep_LivesLostThisSession();

	UPROPERTY(ReplicatedUsing = OnRep_LivesLostThisSession)
	int32 LivesLostThisSession;

	UFUNCTION()
	void OnRep_ReviveEnabled();

	UPROPERTY(ReplicatedUsing = OnRep_ReviveEnabled)
	bool ReviveEnabled = false;

	std::vector<TWeakObjectPtr<APlayerCharacter>> mPlayersPendingRemoval;

	bool mCinematicClassLoading = false;

	UFUNCTION()
	void OnRep_AverageTeamPower();

	UPROPERTY(ReplicatedUsing = OnRep_AverageTeamPower)
	float AverageTeamPower = 0.0f; 

	UFUNCTION()
	void RefreshAverageTeamPower();

	UFUNCTION()
	void OnRep_PartsDiscovered();

	UPROPERTY(ReplicatedUsing = OnRep_PartsDiscovered)
	int32 PartsDiscovered = 0;
};
