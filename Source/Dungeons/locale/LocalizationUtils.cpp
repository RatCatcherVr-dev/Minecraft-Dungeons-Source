// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "LocalizationUtils.h"
#include "DungeonsGameInstance.h"
#include "Engine/Classes/Kismet/KismetInternationalizationLibrary.h"
#include "util/StringUtil.h"

#include <vector>
#include <map>

static int selectedLocaleDebug = 0;
static FString CurrentVOLanguage;
const FString ULocalizationUtils::URLBase = "https://www.minecraft.net/";
const FString ULocalizationUtils::AccessibilityEndpoint = "/accessibility/dungeons";
const FString ULocalizationUtils::UELAEndPoint = "/terms/minecraft-dungeons";

typedef std::pair<DungeonsLocale, std::string> LocaleTuple;
static const std::map<DungeonsLocale, std::string> localeMap =
{ { DungeonsLocale::en, "en" },{ DungeonsLocale::svSE, "sv-SE" } ,{ DungeonsLocale::esMX, "es-MX" }, { DungeonsLocale::esES, "es-ES"}
, { DungeonsLocale::deDE, "de-DE" }, { DungeonsLocale::frFR, "fr-FR" }, { DungeonsLocale::itIT, "it-IT" }
, { DungeonsLocale::jaJP, "ja-JP" }, { DungeonsLocale::koKR, "ko-KR" }, { DungeonsLocale::plPL, "pl-PL" }, { DungeonsLocale::ptBR, "pt-BR" }
, { DungeonsLocale::ptPT, "pt-PT" }, { DungeonsLocale::ruRU, "ru-RU" }, { DungeonsLocale::zhCN, "zh-CN" }, { DungeonsLocale::zhTW, "zh-TW" }};

//D11.PS - this is loc to voice over loc enum. Pt-PT is in English PT-BR is in Portuguese, es-ES is in English and es-MX is in Spanish
//jryden: zhHant will contain audio files for both Chinese locales
static const std::map<FString, DungeonsLocale> voiceOverLocaleMap =
{
	{"en", DungeonsLocale::en}, {"sv-SE", DungeonsLocale::svSE}, {"es-MX", DungeonsLocale::esES}, {"es-ES", DungeonsLocale::en},
	{"de-DE", DungeonsLocale::deDE}, {"fr-FR", DungeonsLocale::frFR},  {"it-IT", DungeonsLocale::en},
	{"ja-JP", DungeonsLocale::jaJP}, {"ko-KR", DungeonsLocale::en}, {"pl-PL", DungeonsLocale::en}, {"pt-BR", DungeonsLocale::ptPT},
	{"pt-PT", DungeonsLocale::en}, {"ru-RU", DungeonsLocale::en}, {"zh-CN", DungeonsLocale::zhCN}, {"zh-TW", DungeonsLocale::zhCN}
};
 

static void FixDefaultLanguageString(FString& DefaultLanguageString)
{
	if (DefaultLanguageString == "en-US")			// D11.Fred : Guess this needs it too, my x-box keeps failing the assert for this
		DefaultLanguageString = "en";
	else if (DefaultLanguageString == "en-GB")		// Fix to reject 'en-GB' which we currently have outdated loc data for. Instead replace with 'en'.
		DefaultLanguageString = "en";
	else if (DefaultLanguageString == "fr-CA")		// Fix to prevent 'fr-CA' from defaulting the loc to English, replacing with 'fr-FR' to use French.
		DefaultLanguageString = "fr-FR";
	else if (DefaultLanguageString == "zh-Hant")		
		DefaultLanguageString = "zh-TW";
	else if (DefaultLanguageString == "zh-Hans")		
		DefaultLanguageString = "zh-CN";
}


bool ULocalizationUtils::VoiceOverLocalizationChanged()
{
	//Check to see if the new language has changed the voice over language from the initial language.
	FString language = UKismetInternationalizationLibrary::GetCurrentLanguage();
	if (language != CurrentVOLanguage)
	{
		auto langIt = voiceOverLocaleMap.find(language);
		ensure(langIt != voiceOverLocaleMap.end());
		auto initialLangIt = voiceOverLocaleMap.find(CurrentVOLanguage);
		ensure(initialLangIt != voiceOverLocaleMap.end());

		if (langIt->second != initialLangIt->second)
		{
			return true;
		}
	}

	return false;
}


void ULocalizationUtils::ChangeLocalizationByString(FString target)
{
	if (CurrentVOLanguage.IsEmpty())
	{
		//D11.PS - Store the initial language, this only needs to be done the first time.
		CurrentVOLanguage = target;
		UE_LOG(LogDungeons, Log, TEXT("ULocalizationUtils: setting current CurrentVOLanguage to %s"), *CurrentVOLanguage);
	}

// D11.IG - Language selection controlled outside of the game by PS4/Xbox/Switch OS.
// Set the lang/locale but do not save to the GGameUserSettingsIni, since settings in this file have higher priority
// than the current OS language in TextLocalisationManager and as a result until the GGameUserSettingsIni is saved again & the game is rebooted
// ALL localized assets (like voice over wave files) in PackageLocalizationCache will be populated using the previous language.
// GGameUserSettingsIni is better suited when game language is changed within the game, not outside at the OS level.
	bool languageSelectionControlledByOS = true;
#if PLATFORM_WINDOWS
	/* Language selection controlled within the game - remember the choice by saving to the settings file */
	languageSelectionControlledByOS = false;
#endif

	UE_LOG(LogDungeons, Log, TEXT("ULocalizationUtils: setting current culture to %s"), *target);
	UKismetInternationalizationLibrary::SetCurrentLanguageAndLocale(target, !languageSelectionControlledByOS);
}

void ULocalizationUtils::ChangeLocalization(DungeonsLocale localeTarget)
{
	auto it = localeMap.find(localeTarget);
	ensure(it != localeMap.end());
	if (it != localeMap.end())
	{
		ChangeLocalizationByString(FString(it->second.c_str()));
	}
}

DungeonsLocale ULocalizationUtils::CurrentLocale() {
	FString currentLocaleString = UKismetInternationalizationLibrary::GetCurrentLocale();
	FixDefaultLanguageString(currentLocaleString);
	auto currentLocale = std::string(TCHAR_TO_UTF8(*currentLocaleString));
	auto it = std::find_if(localeMap.begin(), localeMap.end(),
		[currentLocale](const LocaleTuple p) {
			return p.second == currentLocale; });

	if (it != localeMap.end())
		return it->first;

	return DungeonsLocale::en;
}

FString ULocalizationUtils::GetLocaleString(DungeonsLocale locale) {
	auto it = localeMap.find(locale);
	ensure(it != localeMap.end());
	if (it != localeMap.end())
	{
		return FString(it->second.c_str());
	}

	return FString("");
}

FString ULocalizationUtils::LocStringForWeb(DungeonsLocale locale)
{
	FString localeString;
	if (locale == DungeonsLocale::en)
	{
		localeString = "en-us";
	}	
	else if (locale == DungeonsLocale::zhCN)
	{
		localeString = "zh-hans";
	}
	else if (locale == DungeonsLocale::zhTW)
	{
		localeString = "zh-hant";
	}
	else
	{
		localeString = GetLocaleString(locale);
	}

	return localeString.ToLower();
}

FString ULocalizationUtils::GetAccessibilityURL()
{
	FString fullURL;
	fullURL.Append(URLBase);
	fullURL.Append(LocStringForWeb(CurrentLocale()));
	fullURL.Append(AccessibilityEndpoint);

	return fullURL;
}

FString ULocalizationUtils::GetEULAPage()
{
	FString fullEULAlink;
	fullEULAlink.Append(URLBase);
	fullEULAlink.Append(LocStringForWeb(CurrentLocale()));
	fullEULAlink.Append(UELAEndPoint);

	return fullEULAlink;
}

bool ULocalizationUtils::CompareLanguageISO(const FString& ComparisonISO) {
	return UKismetInternationalizationLibrary::GetCurrentLanguage().Left(2) == ComparisonISO;
}

void ULocalizationUtils::ChangeLocalizationToSystemDefault()
{
	ULocalizationUtils::ChangeLocalizationByString(ValidatedDefaultSystemLanguage());
}

static const std::map<DungeonsLocale, float> readingTimePerCharacter = {
	{DungeonsLocale::en, 0.075f}
, {DungeonsLocale::svSE, 0.075f}
, {DungeonsLocale::esMX, 0.075f}
, {DungeonsLocale::esES, 0.075f}
, {DungeonsLocale::deDE, 0.075f}
, {DungeonsLocale::frFR, 0.075f}
, {DungeonsLocale::itIT, 0.075f}
, {DungeonsLocale::jaJP, 0.15f}
, {DungeonsLocale::koKR, 0.15f}
, {DungeonsLocale::plPL, 0.075f}
, {DungeonsLocale::ptBR, 0.075f}
, {DungeonsLocale::ptPT, 0.075f}
, {DungeonsLocale::ruRU, 0.075f}
, {DungeonsLocale::zhCN, 0.15f}
, {DungeonsLocale::zhTW, 0.15f}
};

float ULocalizationUtils::GetEstimatedReadingDurationForText(const FText& text)
{
	constexpr float MINIMUM_READING_TIME = 0.5f;
	constexpr float FALLBACK_TIME_PER_CHARACTER = 0.15f;
	auto speedIt = readingTimePerCharacter.find(CurrentLocale());
	ensure(speedIt != readingTimePerCharacter.end());
	float speedPerCharacter = (speedIt != readingTimePerCharacter.end()) ? speedIt->second : FALLBACK_TIME_PER_CHARACTER;
	return MINIMUM_READING_TIME + text.ToString().Len() * speedPerCharacter;
}



FString ULocalizationUtils::ValidatedDefaultSystemLanguage()
{
	FString DefaultLanguageString = UKismetSystemLibrary::GetDefaultLanguage();
	FixDefaultLanguageString(DefaultLanguageString);

	auto DefaultLanguage = std::string(TCHAR_TO_UTF8(*DefaultLanguageString));
	auto it = std::find_if(localeMap.begin(), localeMap.end(),
		[DefaultLanguage](const LocaleTuple p) {
		return p.second == DefaultLanguage; });
	if (it != localeMap.end())
		UE_LOG(LogDungeons, Warning, TEXT("Default system language (%s) is not mapped to a DungeonsLocale (enum), English will be used"), *DefaultLanguageString);
	if (it != localeMap.end())
		return DefaultLanguageString;

	return FString("en");
}

DungeonsLocale ULocalizationUtils::GetDefaultLanguageEnum() 
{
	const FString locString = UKismetSystemLibrary::GetDefaultLanguage();
	auto it = std::find_if(localeMap.begin(), localeMap.end(), [locString](const LocaleTuple& tup) {
		return stringutil::toFString(tup.second) == locString;
	});
	if (it != localeMap.end())
		return it->first;

	return DungeonsLocale::en;
}
