/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/

#include "Dungeons.h"
#include "CommonTypes.h"
#include "DungeonsGameInstance.h"
#include "ObjectiveHintTargetComponent.h"

bool UObjectiveHintTargetComponent::IsVisible() const {
	if (auto hintManager = GetHintManager()) {
		return hintManager->IsHintVisible(TriggerHintType);
	}
	else {
		return false;
	}
}

void UObjectiveHintTargetComponent::BeginPlay() {
	UActorComponent::BeginPlay();

	InstanceTracker<UObjectiveHintTargetComponent>::AddInstance(GetWorld(), this);
}

void UObjectiveHintTargetComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	UActorComponent::EndPlay(EndPlayReason);

	InstanceTracker<UObjectiveHintTargetComponent>::RemoveInstance(GetWorld(), this);
}

const UHintManager* UObjectiveHintTargetComponent::GetHintManager() const {
	if (auto gameInstance = GetWorld()->GetGameInstance<UDungeonsGameInstance>()) {
		return gameInstance->GetHintManager();
	}
	else {
		return nullptr;
	}
}
