#include "TextToSpeechImplWin.h"
#include "Engine/Engine.h"
#define WIN32_LEAN_AND_MEAN
#include <sphelper.h>
#include <sapi.h>

static FCriticalSection CriticalSection;

FTextToSpeechAsyncTask::FTextToSpeechAsyncTask(ISpVoice *InputVoice, const FString &InputText)
	: Voice(InputVoice)
	, Text(InputText)
{

}

void FTextToSpeechAsyncTask::DoWork()
{
	CriticalSection.Lock();
	if ( Voice )
		Voice->Speak(*Text, SPF_ASYNC | SPF_PURGEBEFORESPEAK, nullptr);
	CriticalSection.Unlock();
}


TextToSpeechImplWin::TextToSpeechImplWin()
	: Voice(nullptr)
	, SpeakCompleteEventHandle(0)
{
	auto HR = CoCreateInstance(CLSID_SpVoice, nullptr, CLSCTX_ALL, IID_ISpVoice, (void **)&Voice);
	if (!SUCCEEDED(HR))
		Voice = nullptr;
	if (Voice)
	{
		SetNarratorVoice();
		SpeakCompleteEventHandle = Voice->SpeakCompleteEvent();
	}
}

TextToSpeechImplWin::~TextToSpeechImplWin()
{
	if (Voice)
	{
		Voice->Release();
	}
}

void TextToSpeechImplWin::Stop()
{
	Play("");
}

bool TextToSpeechImplWin::IsPlaying()
{
	return SpeakCompleteEventHandle ? 
		WaitForSingleObject(SpeakCompleteEventHandle, 0) != WAIT_OBJECT_0 : false;
}


void TextToSpeechImplWin::Play(const FString &text)
{
	if (!Voice)
	{
		return;
	}

	(new FAutoDeleteAsyncTask<FTextToSpeechAsyncTask>(Voice, text))->StartBackgroundTask();
	if (!text.IsEmpty())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("TTS: %s"), *text));
	}
}

void TextToSpeechImplWin::SetNarratorVoice()
{
	DWORD dataSize;
	RegGetValue(HKEY_CURRENT_USER, L"Software\\Microsoft\\Narrator\\NoRoam", L"SpeechVoice", RRF_RT_REG_SZ, nullptr, NULL, &dataSize);
	wchar_t* regValue = new wchar_t[dataSize + 1];
	RegGetValue(HKEY_CURRENT_USER, L"Software\\Microsoft\\Narrator\\NoRoam", L"SpeechVoice", RRF_RT_REG_SZ, nullptr, (void *)regValue, &dataSize);

	FString str = L"name=";
	str.Append(regValue);
	str.Split("-", &str, NULL, ESearchCase::CaseSensitive);
	str.Append("Desktop");
	delete regValue;
	const TCHAR* voiceName = *str;

	HRESULT hr = S_OK;
	CComPtr<ISpObjectTokenCategory> cpSpCategory = NULL;
	if (SUCCEEDED(hr = SpGetCategoryFromId(SPCAT_VOICES, &cpSpCategory)))
	{
		CComPtr<IEnumSpObjectTokens> cpSpEnumTokens;
		if (SUCCEEDED(hr = cpSpCategory->EnumTokens(voiceName, NULL, &cpSpEnumTokens)))
		{
			CComPtr<ISpObjectToken> pSpTok;
			if (SUCCEEDED(hr = cpSpEnumTokens->Next(1, &pSpTok, NULL)))
			{
				Voice->SetVoice(pSpTok);
				pSpTok.Release();
			}
		}
	}
}
