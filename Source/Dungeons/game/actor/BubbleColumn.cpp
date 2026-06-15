#include "BubbleColumn.h"
#include "DungeonsGameInstance.h"

ABubbleColumn::ABubbleColumn(const FObjectInitializer& objectInitializer) : Super(objectInitializer) {
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	RespawnComponent = CreateDefaultSubobject<URespawnCandidateComponent>(TEXT("RespawnCandidateComponent"));
}

void ABubbleColumn::SetOxygenEnabled(bool enabled) {
	IsEnabled = enabled;
	OnRep_IsEnabled();
}

void ABubbleColumn::SetHot(bool hot) {
	IsHot = hot;
	OnRep_IsHot();
}

void ABubbleColumn::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
	DOREPLIFETIME_CONDITION_NOTIFY(ABubbleColumn, IsEnabled, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ABubbleColumn, IsHot, COND_None, REPNOTIFY_Always);
}