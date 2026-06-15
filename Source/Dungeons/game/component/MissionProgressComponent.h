#pragma once

#include "CoreMinimal.h"
#include "game/realms/Realms.h"
#include "game/mission/MissionInterest.h"
#include "game/levels.h"
#include "game/actor/character/loot/LootDefs.h"
#include "game/difficulty/DifficultyStats.h"
#include "game/difficulty/Difficulty.h"
#include "game/mission/difficulty/MissionDifficulty.h"
#include "game/mission/variation/LevelVariationType.h"
#include "game/mission/request/MissionRequest.h"
#include "game/mission/start/StartMissionStatus.h"
#include "game/mission/state/MissionState.h"
#include "game/dlc/DLCName.h"
#include "online/entitlements/Entitlement.h"
#include "ui/status/ProblemStatus.h"
#include "Components/ActorComponent.h"
#include "game/EndGame/EndGameContent.h"
#include "MissionProgressComponent.generated.h"

class UDungeonsGameInstance;
class UCharacterSerializeComponent;
class APlayerCharacter;
struct EndlessStruggleTier;

namespace missionconsolehandlers { void Unlock(const TArray<FString>&, UWorld*, FOutputDevice&); }

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMissionsCompletedChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMissionsProgressLoaded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDifficultyUnlockedChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnThreatLevelUnlockedChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMissionsChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMissionStateChanged, ELevelNames, mission);

//Debug hooks
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnlockAllChanged);

USTRUCT(BlueprintType)
struct DUNGEONS_API FMissionProgress {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	ELevelNames levelName = ELevelNames::Invalid;

	UPROPERTY(BlueprintReadOnly)
	EGameDifficulty completedDifficulty = EGameDifficulty::Invalid;

	UPROPERTY(BlueprintReadOnly)
	EThreatLevel completedThreatLevel = EThreatLevel::Invalid;

	UPROPERTY(BlueprintReadOnly)
	FEndlessStruggle completedEndlessStruggle;
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FDifficultyProgress {
	GENERATED_BODY()

	//This is saved in save file because we don't want to remove peoples difficulty unlocks
	//just because we add more missions after people have completed a difficulty
	UPROPERTY(BlueprintReadOnly)
	EGameDifficulty unlocked = EGameDifficulty::Invalid;

	UPROPERTY(BlueprintReadOnly)
	EGameDifficulty announced = EGameDifficulty::Invalid;

	UPROPERTY(BlueprintReadOnly)
	EGameDifficulty selected = EGameDifficulty::Invalid;

};

USTRUCT(BlueprintType)
struct DUNGEONS_API FThreatLevelProgress {
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	EThreatLevel unlocked = EThreatLevel::Invalid;

	UPROPERTY(BlueprintReadOnly)
	EThreatLevel announced = EThreatLevel::Invalid;
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FEndGameContentProgress {
	GENERATED_BODY()

	UPROPERTY()
	TSet<EEndGameContentType> alreadyAnnouncedUnlockedContent;
};


USTRUCT(BlueprintType)
struct DUNGEONS_API FMissionRequirement {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	bool Completed;

	UPROPERTY(BlueprintReadOnly)
	FText Description;
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FMapUIState {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Dungeons")
	ERealmName selectedRealm = ERealmName::Invalid;

	UPROPERTY(BlueprintReadWrite, Category = "Dungeons")
	EGameDifficulty selectedDifficulty = EGameDifficulty::Invalid;

	UPROPERTY(BlueprintReadWrite, Category = "Dungeons")
	ELevelNames selectedMission = ELevelNames::Invalid;

	UPROPERTY(BlueprintReadWrite, Category = "Dungeons")
	EThreatLevel selectedThreatLevel = EThreatLevel::Invalid;

	UPROPERTY(BlueprintReadWrite, Category = "Dungeons")
	FVector2D panPosition = FVector2D(0.0f, 0.0f);
};

UCLASS()
class DUNGEONS_API UMissionProgressComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMissionProgressComponent();

	void BeginPlay();

	void OnLocalPawnPossessed();

	UDungeonsGameInstance* GetDungeonsGameInstance() const;	
	void OnEntitlementsProvided(const TArray<FEntitlement>& entitlements);

	//
	// Logic
	//
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsDLCPlayable(EDLCName dlc) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsDLCOwned(EDLCName dlc) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsRealmUnlocked(ERealmName Realm) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsMissionUnlocked(EGameDifficulty Difficulty, ELevelNames LevelName) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsMissionRevealed(EGameDifficulty Difficulty, ELevelNames LevelName) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsMissionCompleted(ELevelNames LevelName) const;	

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsMissionDone(EGameDifficulty Difficulty, ELevelNames LevelName) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsMissionNew(EGameDifficulty Difficulty, ELevelNames LevelName) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool DoesMissionHaveNewRewards(EGameDifficulty Difficulty, ELevelNames LevelName) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsUnlockedAndNotTutorial(EGameDifficulty difficulty, ELevelNames LevelName) const;	

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool DoesMissionHaveNewCapturedMerchants(EGameDifficulty difficulty, ELevelNames levelName) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool DoesMissionHaveNewSecretLocations(EGameDifficulty difficulty, ELevelNames levelName) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool DoesMissionHaveNewEnderEye(EGameDifficulty difficulty, ELevelNames levelName) const;
			
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FStartMissionStatus QueryStartMissionStatus(const FMissionRequest& missionRequest) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool QueryMissionRequestIsStartable(const FMissionRequest& missionRequest) const;
		
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsMissionOfInterest(EMissionInterest interestType, EGameDifficulty difficulty, ELevelNames levelName) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	TArray<EMissionInterest> GetMissionFilteredInterestTypes(const TArray<EMissionInterest>& interestTypes, EGameDifficulty difficulty, ELevelNames levelName) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	TArray<EMissionInterest> GetMissionMarkerInterestTypes(EGameDifficulty difficulty, ELevelNames levelName) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FMissionRequest PreviewMissionSelectionRequest(const FMissionSelection& selection) const;

	//
	// Set Logic
	//
	
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void SetCompletedMission(EGameDifficulty Difficulty, EThreatLevel ThreatLevel, FEndlessStruggle EndlessStruggle, ELevelNames LevelName, int TokensCollected);

	void SetTrialMissions() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void UnlockBonusMission(ELevelNames LevelName);

	UFUNCTION(BlueprintPure, Category = "Dungeons|UnlockKeys", meta = (WorldContext = "WorldContextObject"))
	static bool IsLevelUnlockedForAllLocalPlayers(const UObject* WorldContextObject, ELevelNames level);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|UnlockKeys", meta = (WorldContext = "WorldContextObject"))
	static void UnlockBonusMissionForAllLocalPlayers(UObject* WorldContextObject, ELevelNames level);

	//Will unlock the mission for all players in the session. Must be called on the server
	UFUNCTION(BlueprintCallable, Category = "Dungeons|UnlockKeys", meta = (WorldContext = "WorldContextObject"))
	static void UnlockBonusMissionForAllPlayers(UObject* WorldContextObject, ELevelNames level);

	//
	// Animations
	//

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsMissionDifficultyAndThreatLevelCompleted(EGameDifficulty Difficulty, EThreatLevel ThreatLevel, FEndlessStruggle EndlessStruggle, ELevelNames LevelName) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	int32 GetNumCompletedMissionsAtDifficulty(EGameDifficulty Difficulty) const;

	int32 GetNumCompletedMissionsInSet(TSet<ELevelNames> missions, TOptional<EGameDifficulty> difficulty = {}, TOptional<EThreatLevel> threatlevel = {}) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool GetHasCompletedMissionAtDifficulty(EGameDifficulty Difficulty, ELevelNames LevelName) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const TArray<FMissionProgress>& GetCompletedMissions() const;

	// save to save file to not keep displaying
	UFUNCTION(BlueprintCallable, Category="Dungeons")
	void SetHasDisplayedCompletionNotification() {}

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	EGameDifficulty GetMissionDifficultyCompleted(ELevelNames LevelName) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	EThreatLevel GetMissionThreatLevelCompleted(ELevelNames LevelName) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool AreAllAvailableMissionDifficultiesCompleted(EGameDifficulty difficulty, ELevelNames LevelName) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	void GetTotalEnderEyesCollected(int& outHeldEyes, int& outPortalEyes);

	//Difficulty announcement animations

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsDifficultyUnlocked(EGameDifficulty Difficulty) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsDifficultyAnnounced(EGameDifficulty Difficulty) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsDifficultyCompleted(EGameDifficulty Difficulty) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void SetDifficultyAnnounced(EGameDifficulty Difficulty);
	
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void SetContentAnnounced(EEndGameContentType Type);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	EGameDifficulty GetUnannouncedUnlockedDifficulty() const;
	
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	TArray<EEndGameContentType> GetUnannouncedEndGameContent() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool HasNeverSelectedDifficulty() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void OnDifficultySelected(EGameDifficulty Difficulty);




	//Threat Level requirements
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	EThreatLevel GetMissionValidThreatLevel(ELevelNames LevelName, EGameDifficulty Difficulty, EThreatLevel ThreatLevel) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsMissionThreatLevelValid(ELevelNames LevelName, EThreatLevel ThreatLevel) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsMissionThreatLevelTooLow(ELevelNames LevelName, EThreatLevel ThreatLevel) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsMissionThreatLevelTooHigh(ELevelNames LevelName, EThreatLevel ThreatLevel) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsAnyEndlessStruggleUnlocked(EGameDifficulty Difficulty) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FEndlessStruggle GetHighestCompletedEndlessStruggle() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FEndlessStruggle GetHighestUnlockedEndlessStruggle(EGameDifficulty Difficulty) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsMissionEndlessStruggleUnlocked(EGameDifficulty Difficulty, FEndlessStruggle EndlessStruggle) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FEndlessStruggle GetMissionValidEndlessStruggle(ELevelNames LevelName, EGameDifficulty Difficulty, EThreatLevel ThreatLevel, FEndlessStruggle EndlessStruggle) const;

	//Threat Level announcement animations
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsThreatLevelUnlocked(EThreatLevel ThreatLevel) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsThreatLevelAnnounced(EThreatLevel ThreatLevel) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void SetThreatLevelAnnounced(EThreatLevel ThreatLevel);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	EThreatLevel GetUnannouncedUnlockedThreatLevel() const;



	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	int GetTotalMissionsInDifficultyOfVariationType(EGameDifficulty difficulty, ELevelVariationType variationType) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	int GetCompletedMissionsInDifficultyOfVariationType(EGameDifficulty difficulty, ELevelVariationType variationType) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	TArray<ELevelNames> GetMissionNamesOfInterestInDifficulty(EMissionInterest hintType, EGameDifficulty difficulty) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	int CountMissionsOfInterestInDifficulty(EMissionInterest hintType, EGameDifficulty difficulty) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FMissionInterestCarousel GetBestMissionInterestCarouselInDifficulty(EGameDifficulty difficulty) const;

	

	//Completion query

	bool IsFulfilled(FMissionDifficultyCompletion DifficultyCompletion) const;




	// Map UI State
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FMapUIState GetMapUIState() const;
	
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void SetMapUIState(FMapUIState mapUiState);	

	// Mission state
	TOptional<FMissionState> GetMissionState(ELevelNames) const;
	bool HasMissionState(ELevelNames) const;	
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool ClearMissionState(ELevelNames levelName);
	void SetMissionState(ELevelNames, const FMissionState&);	
	bool GetMissionCanContinue(ELevelNames) const;

	//Requirements
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	TArray<FMissionRequirement> GetMissionUnlockRequirements(EGameDifficulty Difficulty, ELevelNames LevelName) const;

	bool HasUnlockedBonusMission(ELevelNames) const;

	//Delegates

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnUnlockAllChanged OnUnlockAllChanged;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnMissionsCompletedChanged OnMissionsCompletedChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnMissionsProgressLoaded OnMissionsProgressLoaded;
	
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnDifficultyUnlockedChanged OnDifficultyUnlockedChanged;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnThreatLevelUnlockedChanged OnThreatLevelUnlockedChanged;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnMissionsChanged OnMissionsChanged;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnMissionStateChanged OnMissionStateChanged;

#if !UE_BUILD_SHIPPING		
	//Cheat	
	void UnlockAllMissions(bool doUnlock);
	void UnlockAllRealms(bool doUnlock);
	void UnlockAllDLC(bool doUnlock);
#endif

	//Looter state
	const TSet<ELevelNames>& GetUnlockedMissionsSet();
	TArray<EDLCName> GetOwnedDLCs() const;

	const TOptional<game::DifficultyStats>& GetHighestCompletedDifficultyStats();

	//Check if completed
	bool IsMissionDifficultyCompleted(EGameDifficulty Difficulty, ELevelNames LevelName) const;

	const int GetCurrentMissionRecommendedItemPower();

	const TOptional<FMissionDifficulty>& GetHighestCompletedMissionDifficulty() const;

	UCharacterSerializeComponent* GetCharacterSerializeComponent() const;
private:

	const EndlessStruggleTier* GetNextLockedEndlessStruggleTier(EGameDifficulty) const;
	const EndlessStruggleTier* GetHighestUnlockedEndlessStruggleTier(EGameDifficulty) const;

	const FMissionState* GetValidMissionState(ELevelNames) const;
	bool InternalConstForceClearMissionState(ELevelNames) const;

	TOptional<game::DifficultyStats> HighestCompletedDifficultyStats;

	//Logic
	//Difficulty
	bool ShouldDifficultyBeUnlocked(EGameDifficulty Difficulty) const;
	EGameDifficulty GetHighestDifficultyWhichShouldBeUnlocked() const;

	//ThreatLevel
	bool ShouldThreatLevelBeUnlocked(EThreatLevel ThreatLevel) const;
	EThreatLevel GetHighestThreatLevelWhichShouldBeUnlocked() const;


	//Difficulty + Threat + endless struggle combination
	void RefreshHighestCompletedMissionDifficulty();

	//Set
	void SetDifficultyUnlocked(EGameDifficulty Difficulty);
	void SetThreatLevelUnlocked(EThreatLevel ThreatLevel);

	//Progress State
	const FMissionProgress* GetMissionProgress(ELevelNames LevelName) const;
	FMissionProgress& GetOrCreateEditableMissionProgress(ELevelNames LevelName);	

	//Private Logic
	bool HasPreviouslyFoundItemType(FItemId) const;
	bool IsMissionCompletedAtAnyDifficulty(ELevelNames requiredLevel) const;	
	bool DoesMissionContainPreviouslyNotFoundItemTypes(EGameDifficulty, ELevelNames) const;
	bool HasCompletedCurrentTrialAtDifficulty(ELevelNames, EGameDifficulty) const;

	TOptional<TSet<ELevelNames>> mUnlockedMissionsSetCache;
	TOptional<FMissionDifficulty> mHighestCompletedMissionDifficultyCache;

	TArray<FMissionProgress> mCompletedMissions;

	FDifficultyProgress mDifficultyProgress;

	FThreatLevelProgress mThreatLevelProgress;

	FEndGameContentProgress mEndGameContentProgress;

	FMapUIState mMapUIState;

	TSet<ELevelNames> mUnlockedSecretMissions;

	static const TArray<EMissionInterest> MissionInterestTypesToShowOnMissionMarker;	

	APlayerCharacter* GetCharacterOwner() const;

	void SerializeSaveState() const;
	void DeserializeSaveState();

	//Cheat
	bool bUnlockAllMissions = false;
	bool bUnlockAllRealms = false;
	bool bUnlockAllDLC = false;

	friend void missionconsolehandlers::Unlock(const TArray<FString>&, UWorld*, FOutputDevice&);
};
