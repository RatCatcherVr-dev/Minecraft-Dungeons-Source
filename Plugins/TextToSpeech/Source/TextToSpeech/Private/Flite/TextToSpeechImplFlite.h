#pragma once
#include "../../Public/TextToSpeechImplBase.h"
#include "UObject/GCObject.h"
#include "BufferPlayerSynthComponent.h"

struct cst_voice_struct; 

class TextToSpeechImplFlite;

class FAsyncFliteTTS : public FNonAbandonableTask
{
	friend class FAsyncTask<FAsyncFliteTTS>;
public:
	FAsyncFliteTTS(const FString &text, TextToSpeechImplFlite* pImpl)
		: mTextInput(text),
		pImplementationInstance(pImpl)
	{

	}

protected:
	FString mTextInput;
	TextToSpeechImplFlite* pImplementationInstance;

	void DoWork();
	
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FAsyncFliteTTS, STATGROUP_ThreadPoolAsyncTasks);
	}
};

class TextToSpeechImplFlite : public TextToSpeechImplBase
{
public:
	TextToSpeechImplFlite();
	~TextToSpeechImplFlite() override;
	void Play(const FString &text) override;
	void Stop() override;
	bool IsPlaying() override;
	void JobPlay(const FString &text);
	void Initialise() override { BufferPlayer = NewObject<UBufferPlayerSynthComponent>(); BufferPlayer->AddToRoot();	};
	void Deinitialise() override { if (BufferPlayer) { BufferPlayer->RemoveFromRoot(); }BufferPlayer = nullptr; };

private:
	cst_voice_struct *Voice;
	UPROPERTY()
	UBufferPlayerSynthComponent *BufferPlayer;

	TUniquePtr< FAsyncTask< FAsyncFliteTTS > > mPlayTask;
};
