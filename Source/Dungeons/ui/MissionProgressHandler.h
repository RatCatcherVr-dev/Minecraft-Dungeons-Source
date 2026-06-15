#pragma once
#include "GameFramework/Info.h"
#include "game/component/ItemStashComponent.h"
#include "game/component/AwardsGeneratorComponent.h"
#include "DungeonsGameInstance.h"
#include "lovika/LovikaLevelActor.h"
#include "Sound/SoundBase.h"
#include "game/actor/NotificationBroadcastActor.h"
#include "game/objective/EventTypes.h"
#include "game/objective/ObjectiveCameraPanning.h"
#include "MissionProgressHandler.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNewObjectiveStarted, const FText&, Name, const FString&, Id);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnObjectiveComplete, const FText&, Name, const FString&, Id);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCameraPanStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCameraPanDone);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectiveUpdated, int, StepsCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSpecificObjectiveUpdated, const FText&, Name, const FString&, Id, int, StepsCompleted);

UENUM(BlueprintType)
enum class EObjectiveDisplayMode : uint8 {
	InitLevel,
	NewSubobjective,
	NewMainObjective
};

UENUM(BlueprintType)
enum class EAnimationState : uint8 {
	Empty = 0 UMETA(Hidden),
	Intro = 1,
	Start = 2,
	Update = 3,
	Complete = 4,
	Invalid = 5 UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FMissionDetails {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	EObjectiveDisplayMode DisplayMode;
	
	UPROPERTY(BlueprintReadWrite)
	FText Name;

	UPROPERTY(BlueprintReadWrite)
	FText Description;

	UPROPERTY(BlueprintReadWrite)
	int Count;

	UPROPERTY(BlueprintReadWrite)
	int CountMax;

	UPROPERTY(BlueprintReadOnly)
	EAnimationState AnimationState = EAnimationState::Empty;

	UPROPERTY(BlueprintReadOnly)
	EEventType EventType = EEventType::None;

	TOptional<FString> Id;

	// Added temporarily to keep the same behavior. I can't find where (in BP) it's used, if ever
	bool AlwaysShow = false;

	int ObjectiveIndex = -1;

	float UICountdown = 0; // replaces delay in 'Objective' (ArenaBattle) to allow moving camera before triggering UI
};

namespace game { namespace objective {

class Objective;

FMissionDetails makeMissionDetails(const Objective&);

}}

UCLASS(Blueprintable)
class AMissionProgressHandler : public AInfo {
	GENERATED_BODY()
public:

	AMissionProgressHandler();	

	void Tick(float deltaTime) override;

	UPROPERTY(BlueprintAssignable)
	FOnNewObjectiveStarted OnNewObjectiveStarted;

	UPROPERTY(BlueprintAssignable)
	FOnObjectiveComplete OnObjectiveComplete;

	UPROPERTY(BlueprintAssignable)
	FOnObjectiveUpdated OnObjectiveUpdated;

	UPROPERTY(BlueprintAssignable)
	FOnSpecificObjectiveUpdated OnSpecificObjectiveUpdated;

	UPROPERTY(BlueprintAssignable)
	FOnCameraPanStarted OnCameraPanStarted;

	UPROPERTY(BlueprintAssignable)
	FOnCameraPanDone OnCameraPanDone;

	void CameraPanningStarted();
	void CameraPanningCompleted();

	void SetObjective(const game::objective::Objective&);
	void SetDisplayDetails(const FMissionDetails&);

	void SetEmptyObjective();

	void SetPartiallyComplete(int count, int objectiveIndex);

	UFUNCTION(BlueprintCallable)
	void MissionCancelledTravelToLobby();

	void MissionFinished();

	void GameOver();

	UFUNCTION(NetMulticast, Reliable)
	void FinishedObjectiveTag(const FString& tagName);

	void SetObjectiveEventLocation(const FObjectiveEventLocation&, float duration);

	UFUNCTION(BlueprintCallable)
	int GetMissionProgressionCount() const;

	bool WasMissionFinishedTriggeredOnce() const;
	
	void MoveEveryoneToLobbyOnce();
protected:
	UFUNCTION()
	void MoveToNextLevelSettings() const;

	void MoveToNextLevel(const FLevelSettings& levelSettings) const;
	void MoveToLobbyLevel(class UDungeonsGameInstance* pGameInstance, const FLevelSettings& levelSettings) const;
	void MoveToIngameLevel(class UDungeonsGameInstance* pGameInstance, const FLevelSettings& levelSettings) const;

	void MoveToNextLevelDelayed(const FLevelSettings&, float delaySeconds);

	void RestartInMenu() const;

	UFUNCTION(BlueprintImplementableEvent)
	void OnRep_UpdateUI();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastMissionFinished(const FMissionFinishedSummary& missionFinishedSummary, const FLevelSettings& travelLevelSettings);

	UFUNCTION(BlueprintImplementableEvent)
	void OnMissionFinished();

	UFUNCTION(BlueprintImplementableEvent)
	void OnShowMissionVictory(float WaitDurationSeconds);

	UFUNCTION()
	void OnRep_MissionDetails();

	UFUNCTION()
	void OnRep_MissionUpdated();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayMissionEndVideo(class UMediaSource* videoSource);

	UFUNCTION(BlueprintCallable)
	void OnEndVideoFinished();

	//Note - since many of these are changed at once and are all ReplicatedUsing,
	//we will get OnRep_UpdateUI a bit more often than needed. This isn't
	//a huge issue. Should only result in a few more C++ -> BP UI refresh
	//events. @todo NB: It will cause weird animation behavior if they come out of sync.
	//// jryden: don't understand this at all -> currently OnRep_UpdateUI is called 
	//// multiple times on the client and within OnRep_MissionDetails on server - looks very convoluted!
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_UpdateUI)
	bool IsVisible;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MissionDetails)
	FMissionDetails NewMissionDetails;
private:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastObjectiveEventLocation(const FObjectiveEventLocation& location, float duration);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastInstantMoveToNextLevel(const FLevelSettings& travelLevelSettings);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastGameOver(ELevelNames level, const FString& missionStateGuid, const FLevelSettings& travelLevelSettings);

	bool AllowPlayVideo(const MissionDef& missionDef);

	void RepNotifyInternal();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void SetLocalPlayersCompletedMission(const FLevelSettings& levelSettings);
	void RewardLocalPlayers(const FLevelSettings& levelSettings, const TOptional<FString>& trialId);

	void TriggerUI();
	//void CacheOutroMusic();

	void ClearMissionStateForAllLocalPlayers(ELevelNames, const FString& missionStateGuid);

	void ExecuteBeforeVideo();
	void ExecuteAfterVideo();

	TArray<FRewardData> GetRewardItemsForPlayer(APlayerCharacter*, const FLevelSettings&) const;
	float mEmergencyUniqueCap = 1.f;

	FTimerHandle mEndMissionHandle;
	float mRareItemChance = 0.1f;

	// only push the mission progress count for now, but could technically be more info
	// Keep as separate replication to avoid checks on 'NewMissionDetails' and instead assume that
	// updates to it implies a NEW Mission Objective in the UI/audio etc
	UPROPERTY(ReplicatedUsing=OnRep_MissionUpdated)
	int mMissionProgressCount = 0;
	bool mMissionFinishedWasTriggered = false;
	bool HasIssuedReturnToLobby = false;

	float mMissionEndedAtSeconds = 0;
	game::objective::ObjectiveEventHandler ObjectiveEventHandler;
	FLevelSettings mNextLevelSettings;
};
