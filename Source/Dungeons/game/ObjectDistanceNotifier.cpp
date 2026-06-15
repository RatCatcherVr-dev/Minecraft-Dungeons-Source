#include "ObjectDistanceNotifier.h"
#include "util/ActorQuery.h"
#include "TimerManager.h"
#include "util/Algo.hpp"

void UObjectDistanceNotifier::Initialize(AActor* sourceActor, float maxDistance, float timeThreshold) {
	SourceActor = TWeakObjectPtr<AActor>(sourceActor);
	MaxDistance = maxDistance;
	NotifyTimeThreshold = timeThreshold;
	bHasNotified = false;
}

void UObjectDistanceNotifier::SetSourceActor(AActor* sourceActor) {
	SourceActor = TWeakObjectPtr<AActor>(sourceActor);
}

void UObjectDistanceNotifier::Update(float deltaTime, const TArray<AActor*>& targetActors) {
	auto& timerManager = GetWorld()->GetTimerManager();
	if (SourceActor.IsValid() && AreAllFarAway(targetActors)) {
		if (!timerManager.IsTimerActive(TimerHandle) && !bHasNotified) {
			timerManager.SetTimer(TimerHandle, this, &UObjectDistanceNotifier::OnTriggerFarAway, NotifyTimeThreshold);
		}
	}
	else {
		bHasNotified = false;
		timerManager.ClearTimer(TimerHandle);
	}
}

void UObjectDistanceNotifier::OnTriggerFarAway() {
	OnNotifyFarAway.Broadcast();
	bHasNotified = true;
}

bool UObjectDistanceNotifier::AreAllFarAway(const TArray<AActor*>& targetActors) const {
	return targetActors.Num() && algo::all_of(targetActors, RETLAMBDA(actorquery::getActorDistance2D(SourceActor.Get(), it) >= MaxDistance));
}

