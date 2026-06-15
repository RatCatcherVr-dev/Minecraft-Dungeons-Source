#pragma once
#include "../../Public/TextToSpeechImplBase.h"
#include "Async/AsyncWork.h"


struct ISpVoice;

class FTextToSpeechAsyncTask : public FNonAbandonableTask
{
public:
	friend class FAutoDeleteAsyncTask<FTextToSpeechAsyncTask>;

	FTextToSpeechAsyncTask(ISpVoice *InputVoice, const FString &InputText);
	void DoWork();

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FTextToSpeechAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
	}

private:
	ISpVoice* Voice;
	const FString Text;
};



class TextToSpeechImplWin : public TextToSpeechImplBase
{
public:
	TextToSpeechImplWin();
	~TextToSpeechImplWin() override;
	void Play(const FString &text) override;
	void Stop() override;
	bool IsPlaying() override;
private:
	ISpVoice* Voice;
	Windows::HANDLE SpeakCompleteEventHandle;

	void SetNarratorVoice();
};