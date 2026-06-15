#include "ConnectionSubMenuDisplayRowWidget.h"

#include "ConnectionDisplayBaseWidget.h"

FText UConnectionSubMenuDisplayRowWidget::GetTextToSpeech() const {
	return Status->GetTextToSpeech();
}
