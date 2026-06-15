#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LocalizationUtils.generated.h"

class UDungeonsGameInstance;

UENUM(BlueprintType)
enum class DungeonsLocale : uint8
{
	/* Ordering matches that used by the unreal editor */
	en,
	deDE,
	frFR,
	esES,
	esMX,
	itIT,
	jaJP,
	koKR,
	plPL,
	ptPT,
	ptBR,
	ruRU,
	svSE,
	zhCN,
	zhTW,
};

UCLASS()
class DUNGEONS_API ULocalizationUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/* Change Localization at Runtime. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Change Localization"), Category = "Locale")
	static void ChangeLocalization(DungeonsLocale localeTarget);

	/* Change Localization at Runtime. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Change Localization (string)"), Category = "Locale")
	static void ChangeLocalizationByString(FString target);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Voice Over Localization Changed"), Category = "Locale")
	static bool VoiceOverLocalizationChanged();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Locale")
	static DungeonsLocale CurrentLocale();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Locale")
	static FString GetLocaleString(DungeonsLocale locale);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Locale")
	static FString LocStringForWeb(DungeonsLocale locale);

	UFUNCTION(BlueprintCallable, Category = "URL")
	static FString GetAccessibilityURL();

	UFUNCTION(BlueprintCallable, Category = "URL")
	static FString GetEULAPage();

	/* Compare the current language IETF language tag for the provided two letter ISO 639-1 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Locale")
	static bool CompareLanguageISO(const FString& ComparisonISO);

	/* Query the default system language and set our localization accordingly */
	static void ChangeLocalizationToSystemDefault();

	static float GetEstimatedReadingDurationForText(const FText& text);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Locale")
	static DungeonsLocale GetDefaultLanguageEnum(); 

private:
	/* Validate default system language code is compatible with Dungeons, else replace code with a valid one or fallback to English */
	static FString ValidatedDefaultSystemLanguage();

	static const FString URLBase;
	static const FString AccessibilityEndpoint;
	static const FString UELAEndPoint;
};