#include "Dungeons.h"
#include "SubtitleManager.h"

#include "WidgetBlueprintLibrary.h"
#include "Engine.h"
#include "SubtitleOverlayWidget.h"

void USubtitleOverlayWidget::NativeConstruct() {
	Super::NativeConstruct();
	EnableSubtitleUpdates();
	mSubtitlesBinding = FSubtitleManager::GetSubtitleManager()->OnSetSubtitleText().AddUObject(this, &USubtitleOverlayWidget::OnSubtitlesText);

}

void USubtitleOverlayWidget::NativeDestruct() {

	FSubtitleManager::GetSubtitleManager()->OnSetSubtitleText().Remove(mSubtitlesBinding);
	if(!FSubtitleManager::GetSubtitleManager()->OnSetSubtitleText().IsBound())
		DisableSubtitleUpdates();
	Super::NativeDestruct();
}

void USubtitleOverlayWidget::DisableSubtitleUpdates() {
	if(GEngine)
		GEngine->bSubtitlesEnabled = false;
}

void USubtitleOverlayWidget::EnableSubtitleUpdates() {
	if(GEngine)
		GEngine->bSubtitlesEnabled = true;
}
