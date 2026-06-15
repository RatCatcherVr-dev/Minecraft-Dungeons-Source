#include "Dungeons.h"
#include "RelocateComponent.h"
#include "DungeonsGameMode.h"

URelocateComponent::URelocateComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.25f;
}

void URelocateComponent::BeginPlay() {
	Super::BeginPlay();

	RelocateLocation = GetOwner()->GetActorLocation();
}

void URelocateComponent::TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction* thisTickFunction) {
	Super::TickComponent(deltaTime, tickType, thisTickFunction);

	const auto FallSpeedThreshold = 100.f;	
	if (GetOwner()->GetVelocity().Z < -FallSpeedThreshold) {
		CheckCount++;
	} else {
		CheckCount = 0;
	}

	const auto CheckThreshold = 8;
	if (CheckCount >= CheckThreshold) {
		CheckCount = 0;

		GetOwner()->SetActorLocation(RelocateLocation, false, nullptr, ETeleportType::ResetPhysics);
	}	
}
