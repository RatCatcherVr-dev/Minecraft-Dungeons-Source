// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "TextToSpeechBPLibrary.h"
#include "TextToSpeechUtilityWidget.generated.h"

UCLASS()
class TEXTTOSPEECHEDITOR_API UTextToSpeechUtilityWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Text To Speech Editor", meta = (DisplayName = "On Text To Speech Event"))
	void ReceiveTextToSpeechEvent(const FString& inEvent, const FString& inOptionalData);

protected:
	void NativeConstruct() override;
	void NativeDestruct() override;

private:
	FDelegateHandle OnTextToSpeechEvent_Handle;
};
