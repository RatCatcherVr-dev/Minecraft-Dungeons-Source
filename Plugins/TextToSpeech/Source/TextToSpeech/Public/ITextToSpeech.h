#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"
#include "TextToSpeechImplBase.h"

// It is a good practice to declare a Log category for every plugin
DECLARE_LOG_CATEGORY_EXTERN(LogTextToSpeech, Log, All);

class TEXTTOSPEECH_API ITextToSpeech : public IModuleInterface {

public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static TSharedPtr<TextToSpeechImplBase> Get();

private:
	static TSharedPtr<TextToSpeechImplBase> TextToSpeechPtr;
	static ITextToSpeech *Instance;
};
