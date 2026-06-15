#include "Dungeons.h"
#include "DungeonsPropGameMode.h"

void ADungeonsPropGameMode::StartPlay() {
	Super::StartPlay();
	mBehaviorSystem = std::make_unique<UBehaviorSystem>(*GetWorld());
}

void ADungeonsPropGameMode::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	mBehaviorSystem->Update();
}
