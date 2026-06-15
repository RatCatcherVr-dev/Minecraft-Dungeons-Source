#include "TextToSpeechImplXboxOne.h"
#include "WAVFileReader.h"
#include <ppltasks.h>
#include <Robuffer.h>

using Microsoft::WRL::ComPtr;
FCriticalSection TextToSpeechImplXboxOne::CriticalSection;


AudioVoiceCallback::AudioVoiceCallback()
	: BufferEndEvent(CreateEvent(NULL, FALSE, FALSE, NULL)) 
	, Playing(false)
{

}

AudioVoiceCallback::~AudioVoiceCallback()
{
	CloseHandle(BufferEndEvent); 
}

//Called when the voice has just finished playing a contiguous audio stream.
void AudioVoiceCallback::OnStreamEnd()
{ 
	SetEvent(BufferEndEvent); 
}

void AudioVoiceCallback::OnBufferEnd(void * pBufferContext)
{
	Playing = false;
}

void AudioVoiceCallback::OnBufferStart(void * pBufferContext)
{
	Playing = true;
}



TextToSpeechImplXboxOne::TextToSpeechImplXboxOne()
	: MasteringVoice(nullptr)
	, SourceVoice(nullptr)
	, SpeechSynth(nullptr)
	, WavMemory(nullptr)
	, VoiceCallback(nullptr)
	, CurrentTextHash(0)

{
	XAudio2Create(XAudio2.GetAddressOf(), XAUDIO2_DO_NOT_USE_SHAPE);
	if (XAudio2)
	{
#if !UE_BUILD_SHIPPING
		// Enable debugging features
		XAUDIO2_DEBUG_CONFIGURATION debug = {};
		debug.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
		debug.BreakMask = XAUDIO2_LOG_ERRORS;
		XAudio2->SetDebugConfiguration(&debug, 0);
#endif
		XAudio2->CreateMasteringVoice(&MasteringVoice);
	}

	SpeechSynth = ref new SpeechSynthesizer();
}

TextToSpeechImplXboxOne::~TextToSpeechImplXboxOne()
{
	Stop();
	SpeechSynth = nullptr;
}

void TextToSpeechImplXboxOne::Play(const FString &text)
{
	PlayInternal(text);
}

void TextToSpeechImplXboxOne::Stop()
{
	CriticalSection.Lock();
	StopInternal();
	CriticalSection.Unlock();
}

bool TextToSpeechImplXboxOne::IsPlaying()
{
	if (VoiceCallback)
	{
		return VoiceCallback->IsPlaying();
	}
	return false;
}

void TextToSpeechImplXboxOne::StopInternal()
{
	if (SourceVoice)
	{
		XAUDIO2_VOICE_STATE State;
		SourceVoice->GetState(&State);
		if (State.BuffersQueued == 0)
		{
			SourceVoice->Stop();
		}
		SourceVoice->DestroyVoice();
		SourceVoice = nullptr;
		free(WavMemory);
	}

	VoiceCallback = nullptr;
}



void TextToSpeechImplXboxOne::PlayInternal(const FString &speechText)
{
	
 	Platform::String^ text = ref new Platform::String(*speechText);
	uint32 hash = GetTypeHash(*speechText);
	if (hash == CurrentTextHash)
	{
		return;
	}
	CurrentTextHash = hash;

	{
		//D11.SC Wrap the SynthesizeTextToStream task generation in a concurrency lambda to prevent it causing game thread stalls
		concurrency::create_task([this, text] {

			// Generate the audio stream from plain text.
			concurrency::create_task(SpeechSynth->SynthesizeTextToStreamAsync(text)).then([this, text](SpeechSynthesisStream ^speechStream)
			{
				
				//Data reader for stream
				Windows::Storage::Streams::DataReader^ reader = ref new Windows::Storage::Streams::DataReader(speechStream);
				reader->UnicodeEncoding = Windows::Storage::Streams::UnicodeEncoding::Utf8;
				reader->ByteOrder = Windows::Storage::Streams::ByteOrder::LittleEndian;

				concurrency::create_task(reader->LoadAsync(static_cast<unsigned int>(speechStream->Size))).then([this, reader, speechStream, text](concurrency::task<unsigned int> bytesLoaded)
				{
					CriticalSection.Lock();
					{
						//Get buffer from stream reader
						Windows::Storage::Streams::IBuffer^ tempBuffer = reader->ReadBuffer(static_cast<unsigned int>(speechStream->Size));
						// Query the IBufferByteAccess interface
						ComPtr<Windows::Storage::Streams::IBufferByteAccess> bufferByteAccess;
						reinterpret_cast<IInspectable*>(tempBuffer)->QueryInterface(IID_PPV_ARGS(&bufferByteAccess));

						// Retrieve the raw buffer data
						byte* audioData = nullptr;
						bufferByteAccess->Buffer(&audioData);

						StopInternal();

						// Data is WAV formatted, so read directly from memory
						// The audio format will always be 32bit 22khz mono ADPCM
						DX::WAVData waveData;
						DX::LoadWAVAudioInMemoryEx(audioData, speechStream->Size, waveData);

						WAVEFORMATEX wavFormat = {};
						wavFormat.wFormatTag = 1;
						wavFormat.nChannels = 1;
						wavFormat.nSamplesPerSec = 16000;
						wavFormat.nAvgBytesPerSec = 32000;
						wavFormat.nBlockAlign = 2;
						wavFormat.wBitsPerSample = 16;
						wavFormat.cbSize = 0;

						VoiceCallback = std::make_unique<AudioVoiceCallback>();
						XAudio2->CreateSourceVoice(&SourceVoice, &wavFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, VoiceCallback.get());
						// Cache wave data
						WavMemory = (BYTE*)malloc(waveData.audioBytes);
						memcpy(WavMemory, waveData.startAudio, waveData.audioBytes);

						// Submit wave data
						XAUDIO2_BUFFER buffer = {};
						buffer.pAudioData = WavMemory;
						buffer.Flags = XAUDIO2_END_OF_STREAM;       // Indicates all the audio data is being submitted at once
						buffer.AudioBytes = waveData.audioBytes;
						buffer.LoopCount = 0;

						SourceVoice->FlushSourceBuffers();
						SourceVoice->SubmitSourceBuffer(&buffer);
						// Start playing the voice
						SourceVoice->Start();
					}
					CurrentTextHash = 0;
					CriticalSection.Unlock();
				});
				
			});
		});

	}

}