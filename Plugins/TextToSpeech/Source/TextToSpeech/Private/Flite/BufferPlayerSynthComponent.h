#pragma once

#include "CoreMinimal.h"
#include "Components/SynthComponent.h"
#include "BufferPlayerSynthComponent.generated.h"

// ========================================================================
// UBufferReaderSynthComponent
// ========================================================================
UCLASS(ClassGroup = Synth, meta = (BlueprintSpawnableComponent))
class TEXTTOSPEECH_API UBufferPlayerSynthComponent : public USynthComponent
{
    GENERATED_BODY()
    
public:

#ifdef VIRTUAL_SET_AUDIO_BUFFER
    // Sets the audio buffer
    virtual void SetAudioBuffer(int16* AudioBufferPtr, int32 NumSamples, int32 NumChannels, int32 SampleRate) override;
#endif

    // Called when synth is created
    virtual bool Init(int32& SampleRate) override;

    // Called to generate more audio
    virtual int32 OnGenerateAudio(float* OutAudio, int32 NumSamples) override;

    // Returns if the buffer reader has finished playing
    UFUNCTION(BlueprintCallable, Category = "Synth|BufferReader")
    bool IsFinished() const;

	void StopSynth();

    // Returns the playback percentage of the buffer reader
    UFUNCTION(BlueprintCallable, Category = "Synth|BufferReader")
    float GetPlaybackPercentage() const;

private:
    // Array of audio data to read from
    TArray<int16> AudioBuffer;

    // Sample rate of the audio buffer
    int32 BufferSampleRate = 0;

    // Num channels of the audio buffer
    int32 BufferNumChannels = 0;

    // The sample count of how far we are in the buffer
    int64 SampleCounter = 0;

    // Whether or not the buffer was set
    bool bAudioBufferSet = false;
};