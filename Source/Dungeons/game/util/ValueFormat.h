#pragma once

#include "CoreMinimal.h"

namespace valueformat {
	DUNGEONS_API extern FString asMultiplierPercentageChange(float value);
	DUNGEONS_API extern FString asRelativeMultiplierPercentageChange(float value);
	DUNGEONS_API extern FString asRelativeDividerPercentageChange(float value);
	DUNGEONS_API extern FString asRoundedConstant(float value);
	DUNGEONS_API extern FString asSingleDecimalConstant(float value);
	DUNGEONS_API extern FString asConstant(int value);
	DUNGEONS_API extern FString asConstantWord(int value);
	DUNGEONS_API extern FString asEveryRoundedWordSecond(float value);
	DUNGEONS_API extern FString asEveryRoundedSecond(float value);
	DUNGEONS_API extern FString asEverySingleDecimalSecond(float value);
	DUNGEONS_API extern FString asForRoundedWordSecond(float value);
	DUNGEONS_API extern FString asForRoundedSecond(float value);
	DUNGEONS_API extern FString asForSingleDecimalSecond(float value);
	DUNGEONS_API extern FString asDurationRoundedWordSecond(float value);
	DUNGEONS_API extern FString asDurationRoundedSecond(float value);
	DUNGEONS_API extern FString asDurationSingleDecimalSecond(float value);
	DUNGEONS_API extern FString asDamage(float value);
	DUNGEONS_API extern FString asSingleDecimalDamage(float value);
	DUNGEONS_API extern FString asRelativeConstant(float value);
	DUNGEONS_API extern FString asRelativeConstant(int value);
	DUNGEONS_API extern FString asPercentage(float value);
	DUNGEONS_API extern FString asSingleDecimalPercentage(float value);
	DUNGEONS_API extern FString asRoundedFractionalQuarters(float value);
	DUNGEONS_API extern FString asPercentageChance(float value);
	DUNGEONS_API extern FString asEveryRoundedOrdinalSecond(float value);
	DUNGEONS_API extern FString asRoundedOrdinal(float value);
	DUNGEONS_API extern FString asEveryRoundedOrdinal(float value);
	DUNGEONS_API extern FString asRoundedMultiple(float value);
	DUNGEONS_API extern FText   asPercentageText(float value);	
	DUNGEONS_API extern FText	asCompactPlayerNumber(const int localPlayerIndex);
	DUNGEONS_API extern int     healthAsDisplayAmount(float value);
	DUNGEONS_API extern float   healthDisplayScaled(float value);
	DUNGEONS_API extern float   getPeriodForAnyDisplayHealthChange(float HealthPerSecond);	
	DUNGEONS_API extern FText	asNumber(int number);
	DUNGEONS_API extern FText	getAsWordOrNumber(int number);
	DUNGEONS_API extern FText	getOrdinalEverySeconds(float value, const FText& countText);
	DUNGEONS_API extern FText	getEverySeconds(float value, const FText& countText);
	DUNGEONS_API extern FText	getForSeconds(float value, const FText& countText);
	DUNGEONS_API extern FText	getDurationSeconds(float value, const FText& countText);
	DUNGEONS_API extern FText	getOrdinal(int value);
	DUNGEONS_API extern FText	getRoundedFractionalQuarters(float value);
	DUNGEONS_API extern FText	getEveryOrdinal(int value);
	DUNGEONS_API extern FText	getMultipleNoun(int value);
	DUNGEONS_API extern FText	getAsCompactPlayerNumberText(int value);
	DUNGEONS_API extern FText	asCounterLabeled(const FText&, int current, int target);
	DUNGEONS_API extern FText	asCounterLabeled(const FText&, int current);
	DUNGEONS_API extern FText	asCounter(int current, int target);
	DUNGEONS_API extern FText	asDelimiterSeparatedList(const TArray<FText>&, const FText& delimiter, const FText& finalDelimiter);
	DUNGEONS_API extern FText	asCommaSeparatedList(const TArray<FText>&);
	DUNGEONS_API extern FText	asCommaSeparatedOrList(const TArray<FText>&);
	DUNGEONS_API extern FText	asCommaSeparatedAndList(const TArray<FText>&);

}