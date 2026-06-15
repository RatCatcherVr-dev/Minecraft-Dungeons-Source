

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/EngineTypes.h"
#include "ObjectDistanceNotifier.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnNotifyFarAway);

UCLASS()
class DUNGEONS_API UObjectDistanceNotifier : public UObject {
	GENERATED_BODY()
public:
	void Initialize(AActor* SourceActor, float maxDistance, float timeThreshold);
	void SetSourceActor(AActor* sourceActor);

	void Update(float deltaTime, const TArray<AActor*>& targetActors);

	FOnNotifyFarAway OnNotifyFarAway;
private:
	void OnTriggerFarAway();
	bool AreAllFarAway(const TArray<AActor*>& targetActors) const;

	float MaxDistance;
	float NotifyTimeThreshold;

	TWeakObjectPtr<AActor> SourceActor;

	FTimerHandle TimerHandle;

	bool bHasNotified;
};
