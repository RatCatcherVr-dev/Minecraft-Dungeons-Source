// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#include "TextToSpeechUtilityWidget.h"
#include "TextToSpeechBPLibrary.h"

FDelegateHandle OnTextToSpeechEvent_Handle;

void UTextToSpeechUtilityWidget::NativeConstruct()
{
	OnTextToSpeechEvent_Handle = UTextToSpeechBPLibrary::TextToSpeech_OnEvent.AddUObject(this, &UTextToSpeechUtilityWidget::ReceiveTextToSpeechEvent);
	
	Super::NativeConstruct();
}

void UTextToSpeechUtilityWidget::NativeDestruct()
{
	UTextToSpeechBPLibrary::TextToSpeech_OnEvent.Remove(OnTextToSpeechEvent_Handle);
	OnTextToSpeechEvent_Handle.Reset();

	Super::NativeDestruct();
}