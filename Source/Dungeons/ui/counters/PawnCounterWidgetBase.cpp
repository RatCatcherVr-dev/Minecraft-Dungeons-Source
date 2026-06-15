#include "Dungeons.h"
#include "PawnCounterWidgetBase.h"

void UPawnCounterWidgetBase::OnOwnerWasChanged() {
	Super::OnOwnerWasChanged();
	Bind(GetOwningPlayerPawn());
}
