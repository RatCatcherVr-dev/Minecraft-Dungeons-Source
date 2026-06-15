#include "Dungeons.h"
#include "game/util/EnvironmentUtils.h"

#include "MenuGameMode.h"
#include "DungeonsGameInstance.h"
#include "DungeonsGameStateBase.h"

AMenuGameMode::AMenuGameMode() {
	bUseSeamlessTravel = !environment::startedFromEditor();
	GameStateClass = ADungeonsGameStateBase::StaticClass();
}

void AMenuGameMode::StartPlay() {
	Super::StartPlay();

	UDungeonsGameInstance* gi = Cast<UDungeonsGameInstance>(GetGameInstance());

	gi->PlayerNumberCounter = 0;
	gi->FreeSlots.Empty();

}

