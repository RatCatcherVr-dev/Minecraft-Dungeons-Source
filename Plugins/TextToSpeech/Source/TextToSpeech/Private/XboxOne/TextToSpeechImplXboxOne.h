#pragma once
#include "../../Public/TextToSpeechImplBase.h"
#include <xaudio2.h>
#include "Async/AsyncWork.h"

using namespace Windows::Media::SpeechSynthesis;


class AudioVoiceCallback : public IXAudio2VoiceCallback
{
public:
	AudioVoiceCallback();
	virtual ~AudioVoiceCallback();

	bool IsPlaying() { return Playing; }
	virtual void OnStreamEnd() override;
	virtual void OnBufferEnd(void * pBufferContext) override;
	virtual void OnBufferStart(void * pBufferContext) override;
	
	//Unused methods are stubs
	virtual void OnVoiceProcessingPassEnd() override {}
	virtual void OnVoiceProcessingPassStart(UINT32 SamplesRequired) override {}
	virtual void OnLoopEnd(void * pBufferContext) override {}
	virtual void OnVoiceError(void * pBufferContext, HRESULT Error) override {}

private:
	HANDLE BufferEndEvent;
	bool Playing;

};


class TextToSpeechImplXboxOne : public TextToSpeechImplBase
{
public:
	TextToSpeechImplXboxOne();
	~TextToSpeechImplXboxOne() override;
	void Play(const FString &text) override;
	void Stop() override;
	bool IsPlaying() override;

private:
	void PlayInternal(const FString &text);
	void StopInternal();
	static FCriticalSection								CriticalSection;
	Microsoft::WRL::ComPtr<IXAudio2>					XAudio2;
	IXAudio2MasteringVoice*								MasteringVoice;
	IXAudio2SourceVoice*								SourceVoice;
	Windows::Media::SpeechSynthesis::SpeechSynthesizer^	SpeechSynth;
	BYTE*												WavMemory;
	std::unique_ptr<AudioVoiceCallback>					VoiceCallback;
	uint32												CurrentTextHash;
};
