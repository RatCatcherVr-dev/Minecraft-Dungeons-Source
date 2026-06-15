#include "Dungeons.h"
#include "BehaviorSystemActor.h"

ABehaviorSystemActor::ABehaviorSystemActor() {
	PrimaryActorTick.bCanEverTick = true;
}

void ABehaviorSystemActor::Tick(float deltaSeconds) {
	Super::Tick(deltaSeconds);

	if (HasAuthority()) {
		behavior->Update();
	}
}

void ABehaviorSystemActor::BeginPlay() {
	Super::BeginPlay();

	behavior = make_unique<UBehaviorSystem>(*GetWorld());
}
