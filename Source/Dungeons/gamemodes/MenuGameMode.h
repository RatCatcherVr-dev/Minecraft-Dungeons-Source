#pragma once

#include "GameFramework/GameModeBase.h"
#include "MenuGameMode.generated.h"


UCLASS(minimalapi)
class AMenuGameMode : public AGameModeBase {
	GENERATED_BODY()

public:
	AMenuGameMode();
	void StartPlay() override;
};
