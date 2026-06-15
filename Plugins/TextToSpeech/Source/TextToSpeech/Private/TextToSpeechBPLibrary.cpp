// Fill out your copyright notice in the Description page of Project Settings.

#include "TextToSpeechBPLibrary.h"
#include "ITextToSpeech.h"
#include "IConsoleManager.h"
#include "Delegate.h"
#include "Misc/AssertionMacros.h"
#include "Internationalization/Internationalization.h"
#include "Internationalization/Culture.h"

FTextToPlay UTextToSpeechBPLibrary::LastPlayed = FTextToPlay();
bool UTextToSpeechBPLibrary::TextToSpeechEnabled = false;
bool UTextToSpeechBPLibrary::CanInterrupt = true;
float UTextToSpeechBPLibrary::StartDelayMax = 0.5;
float UTextToSpeechBPLibrary::StartDelayCurrent = 0.0;
TArray<FTextToPlay>  UTextToSpeechBPLibrary::TextQueue;
FTextToSpeechEventSignature UTextToSpeechBPLibrary::TextToSpeech_OnEvent;
TArray<FString> UTextToSpeechBPLibrary::SupportedLanguages = { "en" };

UTextToSpeechBPLibrary::UTextToSpeechBPLibrary()
{
	IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("EnableTextToSpeech"),
		TEXT("Allows Text to speech to be used in game"),
		FConsoleCommandDelegate::CreateStatic(SetTextToSpeechEnabled, true),
		ECVF_Default
	);

	IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("DisableTextToSpeech"),
		TEXT("Stops Text to speech being used in game"),
		FConsoleCommandDelegate::CreateStatic(SetTextToSpeechEnabled, false),
		ECVF_Default
	);

	TickDelegate = FTickerDelegate::CreateUObject(this, &UTextToSpeechBPLibrary::Tick);
	TickDelegateHandle = FTicker::GetCoreTicker().AddTicker(TickDelegate);

	//D11.Jphoenix - Then run a manual check on initialise to determine if we should start enabled or disabled.
	TextToSpeechEnabled = UTextToSpeechBPLibrary::IsTextToSpeechSupported();
}

bool UTextToSpeechBPLibrary::Tick(float DeltaTime)
{	
	if (TextQueue.Num() != 0 )
	{
		if (IsTextToSpeechPlaying())
		{
			if (CanInterrupt)
			{
				if (StartDelayCurrent < StartDelayMax)
				{
					StartDelayCurrent += DeltaTime;
				}
				else
				{
					SpeakText();
				}
			}
		}
		else
		{
			if (StartDelayCurrent < StartDelayMax)
			{
				StartDelayCurrent += DeltaTime;
			}
			else
			{
				SpeakText();
			}
		}
	}

	return true;
}

void UTextToSpeechBPLibrary::PlayTextToSpeech(const FText &text, bool bMustFinishPlaying)
{	
	if (GetTextToSpeechEnabled() && IsTextToSpeechSupported())
	{
		if (!text.IsEmpty())
		{
			StartDelayCurrent = 0.0f;

			//D11.JPhoenix - If we're requesting an entry that already exists, remove the original and place it at the end of the queue.
			if (TextQueue.Contains(FTextToPlay(text, bMustFinishPlaying)))
			{
				TextQueue.Remove(FTextToPlay(text, bMustFinishPlaying));
			}

			FTextToPlay textEntry = FTextToPlay(text, bMustFinishPlaying);

			TextQueue.Add(textEntry);
			LogTexttoSpeechEvent("Play Text To Speech Called", textEntry.Text.ToString());
		}
	}
}

void UTextToSpeechBPLibrary::SpeakText()
{
	//While we have more than one entry...
	while (TextQueue.Num() > 1)
	{
		//Is the first entry skippable?
		if (TextQueue[0].MustFinishPlaying == false)
		{
			//Remove it since there's an entry after this one.
			TextQueue.RemoveAt(0);
			continue;
		}

		//If not skippable, break since we must play this next entry.
		break;
	}

	LogTexttoSpeechEvent("Speak Text Called", TextQueue[0].Text.ToString());
	LastPlayed = TextQueue[0];
	ITextToSpeech::Get()->Play(TextQueue[0].Text.ToString());
	CanInterrupt = !TextQueue[0].MustFinishPlaying;
	StartDelayCurrent = 0.0f;
	TextQueue.RemoveAt(0);
}

void UTextToSpeechBPLibrary::SkipTextToSpeech()
{
	if (GetTextToSpeechEnabled())
	{
		//D11.JPhoenix - We want Skip to trigger instantly so set the delay current to max before adding the dummy request.
		StartDelayCurrent = StartDelayMax;

		if (TextQueue.Contains(FTextToPlay(FText::FromString(FString(TEXT(""))), false)))
		{
			TextQueue.Remove(FTextToPlay(FText::FromString(FString(TEXT(""))), false));
		}

		TextQueue.Add(FTextToPlay(FText::FromString(FString(TEXT(""))), false));
		LogTexttoSpeechEvent("Skip Text To Speech Called");
	}
}

void UTextToSpeechBPLibrary::StopTextToSpeech()
{
	ITextToSpeech::Get()->Stop();
	StartDelayCurrent = 0.0;
	TextQueue.Empty();
	LogTexttoSpeechEvent("Stop Text To Speech Called");
}

void UTextToSpeechBPLibrary::PrioritiseLatestTextToSpeech()
{
	if (GetTextToSpeechEnabled())
	{
		if (TextQueue.Num() > 1)
		{
			ITextToSpeech::Get()->Stop();
			StartDelayCurrent = 0.0;

			FTextToPlay temp = TextQueue.Last();
			TextQueue.RemoveAt(TextQueue.Num() - 1);
			TextQueue.Insert(temp, 0);
		}

		LogTexttoSpeechEvent("Prioritise Latest Text To Speech Called");
	}
}

const bool UTextToSpeechBPLibrary::IsTextToSpeechPlaying()
{
	return ITextToSpeech::Get()->IsPlaying();
}

const bool UTextToSpeechBPLibrary::GetTextToSpeechEnabled()
{
	return TextToSpeechEnabled;
}

const TArray <FTextToPlay>& UTextToSpeechBPLibrary::GetTextQueue()
{
	return TextQueue;
}

const FTextToPlay UTextToSpeechBPLibrary::GetLastPlayed()
{
	return LastPlayed;
}

const float UTextToSpeechBPLibrary::GetStartDelayMax()
{
	return StartDelayMax;
}

const float UTextToSpeechBPLibrary::GetStartDelayCurrent()
{
	return StartDelayCurrent;
}

const bool UTextToSpeechBPLibrary::GetCanInterrupt()
{
	return CanInterrupt;
}

const bool UTextToSpeechBPLibrary::IsTextToSpeechSupported()
{
#if !WITH_EDITOR
	return (SupportedLanguages.Contains(FInternationalization::Get().GetCurrentLanguage()->GetName().Left(2)));
#endif

	return false;
}

void UTextToSpeechBPLibrary::SetTextToSpeechEnabled(bool bEnabled)
{
	TextToSpeechEnabled = bEnabled;
}

void UTextToSpeechBPLibrary::LogTexttoSpeechEvent(FString inEvent, FString inOptionalData)
{
	//D11.JPhoenix - Usually not needed, but since we perform logic before broadcast, better to perform the check.
	if (UTextToSpeechBPLibrary::TextToSpeech_OnEvent.IsBound())
	{
		FString optionalDataCombined = "OPTIONAL DATA:\n";
		optionalDataCombined += inOptionalData;

#if WITH_EDITOR
		optionalDataCombined += "\nCALL STACK:\n";
		optionalDataCombined += FFrame::GetScriptCallstack();
#endif

		UTextToSpeechBPLibrary::TextToSpeech_OnEvent.Broadcast(inEvent, optionalDataCombined);
	}
}

bool FTextToPlay::operator==(const FTextToPlay& arg) const {
	return Text.EqualTo(arg.Text) && MustFinishPlaying == arg.MustFinishPlaying;
}
