#include "Dungeons.h"
#include "PostGameWidget.h"
#include "DungeonsGameState.h"
#include "DungeonsGameInstance.h"

const FMissionFinishedSummary& UPostGameWidget::GetMissionFinishedSummary() const {
	const auto* gameInstance = GetGameInstance<UDungeonsGameInstance>();
	const auto& maybeSummary = gameInstance->GetMissionFinishedSummary();
	ensure(maybeSummary.IsSet() && "HasInfoToShow need to be checked before calling this!");
	return maybeSummary.GetValue();	
}

bool UPostGameWidget::HasInfoToShow() const {
	const auto* gameInstance = GetGameInstance<UDungeonsGameInstance>();
	return gameInstance->GetMissionFinishedSummary().IsSet();
}

void UPostGameWidget::Reset() {
	auto* gameInstance = GetGameInstance<UDungeonsGameInstance>();
	gameInstance->ClearMissionFinishedSummary();	
}