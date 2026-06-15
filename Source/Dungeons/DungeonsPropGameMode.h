#pragma once

#include "DungeonsGameMode.h"
#include "game/ai/BehaviorSystem.h"
#include "DungeonsPropGameMode.generated.h"

UCLASS(minimalapi)
class ADungeonsPropGameMode : public ADungeonsGameMode {
	GENERATED_BODY()
public:

	void StartPlay() override;

protected:

	void Tick(float DeltaSeconds) override;

private:

	Unique<UBehaviorSystem> mBehaviorSystem;
};
