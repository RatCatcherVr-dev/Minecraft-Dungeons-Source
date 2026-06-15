#pragma once
#include "GameFramework/Info.h"

#include "DungeonsGameInstance.h"
#include "lovika/LovikaLevelActor.h"
#include "Sound/SoundBase.h"
#include "game/objective/EventTypes.h"
#include "game/objective/Objective.h"

#include "MissionPresentationHandler.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTriggerIntroMusic, const USoundCue*, MusicTrack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTriggerOutroMusic, const USoundCue*, MusicTrack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectiveMusicStart, const USoundCue*, MusicTrack);


USTRUCT(BlueprintType)
struct DUNGEONS_API FMissionPresentation {
	GENERATED_BODY()
	FMissionPresentation() = default;
	FMissionPresentation(const game::objective::Objective& obj, ELevelNames levelname);

	UPROPERTY(BlueprintReadOnly)
	EEventType EventType = EEventType::None;
	UPROPERTY(BlueprintReadOnly)
	float UICountdown = 0;
	UPROPERTY(BlueprintReadOnly)
	int ObjectiveIndex = 0;
	UPROPERTY(BlueprintReadOnly)
	bool TriggerMusicEvents = false;
	
	UPROPERTY(BlueprintReadOnly)
	FString Id;

public:
	void write() const;

	bool operator== (const FMissionPresentation& other) const {
		return Id != "" && other.Id != ""? Id == other.Id : ObjectiveIndex == other.ObjectiveIndex;
	}

	bool operator!= (const FMissionPresentation& other) const {
		return !(*this == other);
	}
};

UCLASS(Blueprintable)
class UMissionPresentationHandler : public UActorComponent {
	GENERATED_BODY()

public:
	UMissionPresentationHandler();

	UPROPERTY(BlueprintAssignable)
	FOnTriggerIntroMusic OnTriggerIntroMusic;

	UPROPERTY(BlueprintAssignable)
	FOnTriggerOutroMusic OnTriggerOutroMusic;

	UPROPERTY(BlueprintAssignable)
	FOnObjectiveMusicStart OnObjectiveMusicStart;

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void ObjectiveStarted(const game::objective::Objective&);
	void ObjectiveFinished(const game::objective::Objective&);
	
	void SetOOBMissionBoundsLimit(FBox arenaTileBounds);

protected:
	UFUNCTION()
	void OnRep_PresentationData();
	UFUNCTION()
	void OnRep_SetMissionBoundingBox();

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PresentationData)
	FMissionPresentation mPresentationData;

	UFUNCTION(NetMulticast, Reliable)
	void EndCurrentObjective();

private:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdatePresentationData(const FMissionPresentation& presentationData);
	void updateStateMusicPlayback();
	void RepNotifyInternal(); // needed?
	void TriggerMusicLoop();

	UPROPERTY(ReplicatedUsing = OnRep_SetMissionBoundingBox)
	FBox mMissionBoundingBox;

};
