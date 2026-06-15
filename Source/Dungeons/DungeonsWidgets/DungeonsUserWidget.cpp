#include "DungeonsUserWidget.h"

void UDungeonsUserWidget::OnOwnerWasChanged() {
	RegisterInputComponent();
	OnSetOwner();
}

void UDungeonsUserWidget::OnOwnerIsChanging() {
	UnregisterInputComponent();
}
