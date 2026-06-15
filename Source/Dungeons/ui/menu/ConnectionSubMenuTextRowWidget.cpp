#include "ConnectionSubMenuTextRowWidget.h"

#include "TextBlock.h"

void UConnectionSubMenuTextRowWidget::Update(const FText& service, const FText& status) {
	Service->SetText(service);
	Status->SetText(status);
}
