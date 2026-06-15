// © 2020 Mojang Synergies AB. TM Microsoft Corporation.


#include "HotbarConnectionStatusWidget.h"
#include "online/sessions/OnlineUtil.h"
#include "online/seasons/LiveOps.h"
#include "TextToSpeechBPLibrary.h"

void UHotbarConnectionStatusWidget::NativeConstruct() {
	Super::NativeConstruct();
	if (auto liveOps = online::getLiveOps(GetGameInstance())) {
		UTextToSpeechBPLibrary::PlayTextToSpeech(GetTextToSpeech(), false);
	}
}

void UHotbarConnectionStatusWidget::UpdateWidgets() {
	Super::UpdateWidgets();

	PlayAnimation(TextFadeout);
}

void UHotbarConnectionStatusWidget::OnStateChanged(EMinecraftAPIConnectionStatus status) {
	if (status == EMinecraftAPIConnectionStatus::TimingOut) {
		UTextToSpeechBPLibrary::PlayTextToSpeech(GetTextToSpeech(), false);
	}
}
