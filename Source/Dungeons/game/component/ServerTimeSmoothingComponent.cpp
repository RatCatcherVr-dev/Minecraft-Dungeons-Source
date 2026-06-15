
// © 2020 Mojang Synergies AB. TM Microsoft Corporation.


#include "ServerTimeSmoothingComponent.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerState.h"

UServerTimeSmoothingComponent::UServerTimeSmoothingComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = ETickingGroup::TG_PrePhysics;
}

// Called every frame
void UServerTimeSmoothingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const float desiredTime = GetServerTimeSeconds();
	const float tickedTime = SmoothedServerTime + DeltaTime;
	const float delta = desiredTime - tickedTime;

	if (FMath::Abs(delta) > ToleranceStartSeconds && !bIsSmoothing) {
		bIsSmoothing = true;
	}

	if (bIsSmoothing) {
		if (DeltaTime < SmoothingTimeSeconds) {
			//We reduce our speed exponentially when we are ahead, but never stop.
			if (delta < 0.f) {
				SmoothedServerTime += DeltaTime * ( 1.f - DeltaTime / SmoothingTimeSeconds);
			} else {
				SmoothedServerTime = desiredTime - delta * DeltaTime / SmoothingTimeSeconds;
			}
		} else {
			SmoothedServerTime = desiredTime;
		}

		if (FMath::IsNearlyEqual(SmoothedServerTime, desiredTime, ToleranceStopSeconds)) {
			SmoothedServerTime = desiredTime;
			bIsSmoothing = false;
		}
	} else {
		SmoothedServerTime = tickedTime;
	}
}

void UServerTimeSmoothingComponent::BeginPlay() {
	Super::BeginPlay();

	SmoothedServerTime = GetServerTimeSeconds();
}

void UServerTimeSmoothingComponent::SetComponentTickEnabled(bool bEnabled) {
	if(bEnabled) SmoothedServerTime = GetServerTimeSeconds();

	Super::SetComponentTickEnabled(bEnabled);
}

float UServerTimeSmoothingComponent::GetLeadTimeSeconds() const {
	if (auto playerController = GetWorld()->GetFirstPlayerController()) {
		if (playerController->PlayerState) {
			return playerController->PlayerState->Ping * 4 / 1000.f;
		}
	}

	return 0.f;
}

float UServerTimeSmoothingComponent::GetServerTimeSeconds() const {
	if (const auto* gameState = GetWorld()->GetGameState()) {
		const float serverTime = gameState->GetServerWorldTimeSeconds();

		return serverTime + GetLeadTimeSeconds();
	}

	return 0.f;
}
