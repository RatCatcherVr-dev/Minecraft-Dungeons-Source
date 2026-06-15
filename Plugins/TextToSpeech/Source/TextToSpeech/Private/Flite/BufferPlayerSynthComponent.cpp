// Copyright Epic Games, Inc. All Rights Reserved.
#include "BufferPlayerSynthComponent.h"
#include "GenericPlatform/GenericPlatformFile.h"

bool UBufferPlayerSynthComponent::Init(int32& SampleRate)
{
    if (!BufferSampleRate)
    {
        UE_LOG(LogTemp, Error, TEXT("Need to call SetAudioBuffer before playing the Buffer Reader Synth Component"));
        return false;
    }
    SampleCounter = 0;

    // Set the synth component's channel count to the buffer's channel count
    NumChannels = BufferNumChannels;

    // Sets the sample rate of the synth based on the sample rate of the buffer
    SampleRate = BufferSampleRate;

    return true;
}

#ifdef VIRTUAL_SET_AUDIO_BUFFER
void UBufferPlayerSynthComponent::SetAudioBuffer(int16* AudioBufferPtr, int32 NumSamples, int32 NumChan, int32 SampleRate)
{
    if (NumSamples == 0 || NumChan <= 0 || NumChan > 2 || SampleRate <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid parameters for SetAudioBuffer"));
        return;
    }

	SampleCounter = 0;

    // Copy the audio buffer data over to the internal array
	AudioBuffer.Reset();

    // Note we could immediately convert the input int16 audio formatted data to float right away but that would double the internal buffer size.
    // However, that would be cheaper to play back as we'd just need to do a copy and not worry about format conversion.
    AudioBuffer.Append(AudioBufferPtr, NumSamples);

    // Set the data so when the synth component initializes we're 
    BufferNumChannels = NumChan;
    BufferSampleRate = SampleRate;
}
#endif

bool UBufferPlayerSynthComponent::IsFinished() const
{
    // Will return true once the audio buffer reader has finished
    return SampleCounter >= AudioBuffer.Num();
}

void UBufferPlayerSynthComponent::StopSynth()
{
	AudioBuffer.Reset();
	Stop();
}

float UBufferPlayerSynthComponent::GetPlaybackPercentage() const
{
    if (AudioBuffer.Num() > 0)
    {
        return 100.0f * FMath::Clamp((float)SampleCounter / AudioBuffer.Num(), 0.0f, 1.0f);
    }
    return 0.0f;
}

int32 UBufferPlayerSynthComponent::OnGenerateAudio(float* OutAudio, int32 NumSamples)
{
    if (AudioBuffer.Num() > 0 && SampleCounter < AudioBuffer.Num())
    {
        // Compute the number of samples to copy to output buffer
        const int32 NumSamplesToCopy = FMath::Min(AudioBuffer.Num() - SampleCounter, (int64)NumSamples);

        // We use the raw ptr usually for audio buffer iterations to avoid overhead of index bounds checking per array access
        int16* AudioBufferPtr = AudioBuffer.GetData();

        for (int32 SampleIndex = 0; SampleIndex < NumSamplesToCopy; ++SampleIndex)
        {
            // Write to output but convert to float from int16
            // Note: this could be sped up using a SIMD operation. We are building a SIMD library in 
            // Engine\Source\Runtime\SignalProcessing\Public\DSP\BufferVectorOperations.h to do this sort of thing much faster
            OutAudio[SampleIndex] = AudioBufferPtr[SampleIndex + SampleCounter] / 32768.0f;
        }

        // Update the sample counter
        SampleCounter += NumSamplesToCopy;
		return NumSamplesToCopy;
    }

    return 0;
}
