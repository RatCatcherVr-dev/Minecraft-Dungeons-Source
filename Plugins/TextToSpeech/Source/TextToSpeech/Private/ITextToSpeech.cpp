#include "../Public/ITextToSpeech.h"

#ifdef USE_FLITE_TTS
#include "Flite/TextToSpeechImplFlite.h"
#elif PLATFORM_WINDOWS
#include "Windows/TextToSpeechImplWin.h"
#elif PLATFORM_XBOXONE
#include "XboxOne/TextToSpeechImplXboxOne.h"
#endif

DEFINE_LOG_CATEGORY(LogTextToSpeech);


ITextToSpeech*						ITextToSpeech::Instance = nullptr;
TSharedPtr<TextToSpeechImplBase>	ITextToSpeech::TextToSpeechPtr = nullptr;

void ITextToSpeech::StartupModule()
{
	Instance = this;
#ifdef USE_FLITE_TTS
	TextToSpeechPtr = MakeShareable(new TextToSpeechImplFlite());
#elif PLATFORM_WINDOWS
	TextToSpeechPtr = MakeShareable(new TextToSpeechImplWin());
#elif PLATFORM_XBOXONE
	TextToSpeechPtr = MakeShareable(new TextToSpeechImplXboxOne());
#else
	TextToSpeechPtr = MakeShareable(new TextToSpeechImplBase());
#endif
}


void ITextToSpeech::ShutdownModule()
{
}

// This method will give you the instance of the plugin implementation.
TSharedPtr<TextToSpeechImplBase> ITextToSpeech::Get()
{
	// First of all - check if the module has been loaded and load it if hasn't.
	// On the IOS and Android the plugin is loaded at the game startup. On the Windows Editor it is not,
	// so it will be created when used first.
	if (Instance == nullptr)
	{
		check(IsInGameThread());
		FModuleManager::LoadModuleChecked<ITextToSpeech>("TextToSpeech");
	}

	// Sanity check if everything was properly created
	check(Instance != nullptr);
	check(TextToSpeechPtr.IsValid() == true);

	// Return the pointer to the module implementation.
	return Instance->TextToSpeechPtr;
}


IMPLEMENT_MODULE(ITextToSpeech, TextToSpeech)