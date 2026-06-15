#include "TextToSpeechImplFlite.h"

#ifdef USE_FLITE_TTS

#include "Misc/CoreDelegates.h"
#include "flite.h"

TextToSpeechImplFlite::TextToSpeechImplFlite()
	: BufferPlayer(nullptr)
{
	flite_init();
	flite_set_lang_list();

	if (!flite_voice_list) 
	{
		flite_set_voice_list("");
	}

	Voice = flite_voice_select(nullptr);

}

TextToSpeechImplFlite::~TextToSpeechImplFlite()
{
	if (mPlayTask)
	{
		if (!mPlayTask->Cancel())
		{
			mPlayTask->EnsureCompletion();
		}
	}
}

void TextToSpeechImplFlite::Play(const FString &text)
{
	if (BufferPlayer)
	{
		Stop();
		mPlayTask = MakeUnique< FAsyncTask< FAsyncFliteTTS > >(text, this);
		if (mPlayTask)
		{
			mPlayTask->StartBackgroundTask();
		}
	}
}

void TextToSpeechImplFlite::Stop()
{
	if (mPlayTask)
	{
		if (!mPlayTask->Cancel())
		{
			mPlayTask->EnsureCompletion();
		}
		mPlayTask = nullptr;
	}

	//D11.JPhoenix - Manually call stop on Synth component when TTS stop is called.
	if (BufferPlayer)
	{
		BufferPlayer->StopSynth();
	}
}

bool TextToSpeechImplFlite::IsPlaying()
{
	if (BufferPlayer)
	{
		return !BufferPlayer->IsFinished();
	}

	return false;
}

void TextToSpeechImplFlite::JobPlay(const FString &text)
{
	if (BufferPlayer)
	{
		auto TextToSpeechData = flite_text_to_wave(TCHAR_TO_ANSI(*text), Voice);

		if (TextToSpeechData && TextToSpeechData->samples)
		{
			//Push the actual playing of the wave to the game thread
			TWeakObjectPtr<UBufferPlayerSynthComponent> WeakSynthComponent = BufferPlayer;
			FFunctionGraphTask::CreateAndDispatchWhenReady([TextToSpeechData, WeakSynthComponent]()
			{
				if (TextToSpeechData)
				{
					if (WeakSynthComponent.IsValid())
					{
						int16* pData = reinterpret_cast<int16*>(TextToSpeechData->samples);
#ifdef VIRTUAL_SET_AUDIO_BUFFER
						WeakSynthComponent->SetAudioBuffer(pData, TextToSpeechData->num_samples, TextToSpeechData->num_channels, TextToSpeechData->sample_rate);
#endif
						WeakSynthComponent->Start();
					}

					delete_wave(TextToSpeechData);
				}

			}, TStatId(), nullptr, ENamedThreads::GameThread);
		}
	}
}

void FAsyncFliteTTS::DoWork()
{
	if (pImplementationInstance)
	{
		pImplementationInstance->JobPlay(mTextInput);
	}
}

#endif

