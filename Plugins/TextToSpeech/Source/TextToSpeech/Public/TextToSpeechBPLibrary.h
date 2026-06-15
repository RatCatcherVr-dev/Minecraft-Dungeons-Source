// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Runtime/CoreUObject/Public/UObject/TextProperty.h"
#include "Runtime/Core/Public/Containers/Ticker.h"
#include "Engine/EngineTypes.h"
#include "Object.h"
#include "WeakObjectPtr.h"
#include "Delegate.h"
#include "DelegateCombinations.h"
#include "TextToSpeechBPLibrary.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE_TwoParams(FTextToSpeechEventSignature, const FString&, const FString&);

USTRUCT(BlueprintType)
struct FTextToPlay
{
	GENERATED_USTRUCT_BODY()
	FTextToPlay() {}
	FTextToPlay(FText inText, bool inMustFinishPlaying) : Text(inText), MustFinishPlaying(inMustFinishPlaying) {}

	UPROPERTY(BlueprintReadWrite)
	FText Text;

	UPROPERTY(BlueprintReadWrite)
	bool MustFinishPlaying;

	bool operator == (const FTextToPlay& arg) const;
};

UCLASS()
class TEXTTOSPEECH_API UTextToSpeechBPLibrary : public UObject
{
	GENERATED_BODY()

public:
	UTextToSpeechBPLibrary();

	/*Enables or Disables Text To Speech*/
	UFUNCTION(BlueprintCallable, Category = "TextToSpeech", DisplayName = "Set Text To Speech Enabled")
	static void SetTextToSpeechEnabled(bool bEnabled);

	/*Creates a Text To Speech request and appends it to the queue*/
	UFUNCTION(BlueprintCallable, Meta = (AutoCreateRefTerm = "Text"), Category = "TextToSpeech", DisplayName = "Play Text To Speech")
	static void PlayTextToSpeech(const FText &text, bool bMustFinishPlaying);

	/*Stops Text To Speech playback and empties the queue. NOTE: Since this empties the queue, all Text To Speech triggers up to that point will be lost*/
	UFUNCTION(BlueprintCallable, Category = "TextToSpeech", DisplayName = "Stop Text To Speech")
	static void StopTextToSpeech();

	/*Skips Text To Speech playback if it is skippable. NOTE: This will not skip any phrases that must finish playing, and will not empty the queue*/
	UFUNCTION(BlueprintCallable, Category = "TextToSpeech", DisplayName = "Skip Text To Speech")
	static void SkipTextToSpeech();

	UFUNCTION(BlueprintCallable, Category = "TextToSpeech", DisplayName = "Prioritise Latest Text To Speech")
	static void PrioritiseLatestTextToSpeech();

#pragma region Getters

	UFUNCTION(BlueprintCallable, Category = "TextToSpeech", DisplayName = "Is Text To Speech Playing")
	static const bool IsTextToSpeechPlaying();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TextToSpeech", DisplayName = "Get Text To Speech Enabled")
	static const bool GetTextToSpeechEnabled();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TextToSpeech", DisplayName = "Get Text To Speech Queue")
	static const TArray <FTextToPlay>& GetTextQueue();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TextToSpeech", DisplayName = "Get Text To Speech Last Played")
	static const FTextToPlay GetLastPlayed();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TextToSpeech", DisplayName = "Get Text To Speech Start Delay Max")
	static const float GetStartDelayMax();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TextToSpeech", DisplayName = "Get Text To Speech Start Delay Current")
	static const float GetStartDelayCurrent();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TextToSpeech", DisplayName = "Get Text To Speech Can Interrupt")
	static const bool GetCanInterrupt();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TextToSpeech", DisplayName = "Is Text To Speech Supported")
	static const bool IsTextToSpeechSupported();

#pragma endregion

	static FTextToSpeechEventSignature TextToSpeech_OnEvent;

private:
	static void LogTexttoSpeechEvent(FString inEvent, FString inOptionalData = "");

	bool Tick(float DeltaTime);

	FTickerDelegate							TickDelegate;
	FDelegateHandle							TickDelegateHandle;
	FTimerHandle							SpeechTimeHandler;

	void SpeakText();

	static FTextToPlay						LastPlayed;
	static float							StartDelayMax;
	static float							StartDelayCurrent;
	static bool								TextToSpeechEnabled;
	static bool								CanInterrupt;
	static TArray < FTextToPlay >			TextQueue;
	static TArray < FString >				SupportedLanguages;
};
