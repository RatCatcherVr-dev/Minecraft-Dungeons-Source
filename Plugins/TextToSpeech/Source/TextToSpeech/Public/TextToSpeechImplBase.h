#pragma once
#include "CoreMinimal.h"

class TEXTTOSPEECH_API TextToSpeechImplBase
{
public:
	virtual ~TextToSpeechImplBase() {}
	virtual void Play(const FString &text) {}
	virtual void Stop() {}
	virtual bool IsPlaying() { return false; }
	virtual void Initialise() { }
	virtual void Deinitialise() { }
};