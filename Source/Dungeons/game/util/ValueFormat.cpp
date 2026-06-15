
#include "Dungeons.h"
#include "ValueFormat.h"
#include <Kismet/KismetStringLibrary.h>
#include "util/Algo.hpp"

namespace valueformat {
	FString asMultiplierPercentageChange(float value) {
		const float multiplier_change = value > 1.0f ? value - 1.0f : 1.0f - value;
		return FString::Printf(TEXT("%i%%"), FMath::RoundToInt(multiplier_change * 100));
	}
	FString asRelativeMultiplierPercentageChange(float value) {
		const float multiplier_change = value > 1.0f ? value - 1.0f : 1.0f - value;
		FString prefix = value >= 1.0f ? "+" : "-";
		return prefix.Append(FString::Printf(TEXT("%i%%"), FMath::RoundToInt(multiplier_change * 100)));
	}
	FString asRelativeDividerPercentageChange(float value) {
		return asRelativeMultiplierPercentageChange(1 / value);
	}
	FText asNumber(int number) {
		return FText::FromString(FString::FromInt(number));
	}
	FString asConstant(int value) {		
		return FString::Printf(TEXT("%i"), value);
	}
	FString asConstantWord(int value) {
		return getAsWordOrNumber(value).ToString();
	}
	FString asEveryRoundedWordSecond(float value) {
		const int rounded = FMath::RoundToInt(value);
		return getEverySeconds(rounded, getAsWordOrNumber(rounded)).ToString();
	}
	FString asEveryRoundedSecond(float value) {
		const int rounded = FMath::RoundToInt(value);
		return getEverySeconds(rounded, FText::FromString(asConstant(rounded))).ToString();
	}
	FString asEverySingleDecimalSecond(float value) {
		return getEverySeconds(value, FText::FromString(asSingleDecimalConstant(value))).ToString();
	}	
	FString asForRoundedWordSecond(float value) {
		const int rounded = FMath::RoundToInt(value);
		return getForSeconds(rounded, getAsWordOrNumber(rounded)).ToString();
	}
	FString asForRoundedSecond(float value) {
		const int rounded = FMath::RoundToInt(value);
		return getForSeconds(rounded, FText::FromString(asConstant(rounded))).ToString();
	}
	FString asForSingleDecimalSecond(float value) {
		return getForSeconds(value, FText::FromString(asSingleDecimalConstant(value))).ToString();
	}
	FString asDurationRoundedWordSecond(float value){
		const int rounded = FMath::RoundToInt(value);
		return getDurationSeconds(rounded, getAsWordOrNumber(rounded)).ToString();
	}
	FString asDurationRoundedSecond(float value) {
		const int rounded = FMath::RoundToInt(value);
		return getDurationSeconds(rounded, FText::FromString(asConstant(rounded))).ToString();
	}
	FString asDurationSingleDecimalSecond(float value) {
		return getDurationSeconds(value, FText::FromString(asSingleDecimalConstant(value))).ToString();
	}
	FString asEveryRoundedOrdinalSecond(float value){
		const int rounded = FMath::RoundToInt(value);
		return getOrdinalEverySeconds(rounded, getOrdinal(rounded)).ToString();
	}
	FString asRoundedOrdinal(float value) {
		const int rounded = FMath::RoundToInt(value);
		return getOrdinal(rounded).ToString();
	}
	FString asEveryRoundedOrdinal(float value) {
		const int rounded = FMath::RoundToInt(value);		
		return getEveryOrdinal(rounded).ToString();
	}
	FString asRoundedMultiple(float value) {
		return getMultipleNoun(FMath::RoundToInt(value)).ToString();
	}
	FString asSingleDecimalConstant(float value) {
		return FString::Printf(TEXT("%.1f"), value);
	}
	FString asRelativeConstant(int value) {
		FString prefix = value >= 0 ? "+" : "-";
		return prefix.Append(FString::Printf(TEXT("%i"), value));
	}
	FString asRoundedConstant(float value) {
		const int constant = FMath::RoundToInt(value);
		return FString::Printf(TEXT("%i"), constant);
	}
	FString asRelativeConstant(float value) {
		const int constant = FMath::RoundToInt(value);
		FString prefix = value >= 0.0f ? "+" : "-";
		return prefix.Append(FString::Printf(TEXT("%i"), constant));
	}
	FString asPercentage(float value) {
		return FString::Printf(TEXT("%i%%"), FMath::RoundToInt(value * 100));
	}
	FString asSingleDecimalPercentage(float value) {
		return FString::Printf(TEXT("%.1f%%"), value * 100);
	}
	FString asRoundedFractionalQuarters(float value){
		return getRoundedFractionalQuarters(value).ToString();
	}
	FString asPercentageChance(float value) {
		return asPercentage(value);
	}
	FText asPercentageText(float value) {
		return FText::AsCultureInvariant(asPercentage(value));
	}
	FString asDamage(float value) {
		const int constant = healthAsDisplayAmount(value);
		return FString::Printf(TEXT("%i"), constant);
	}
	FString asSingleDecimalDamage(float value) {
		const float constant = healthDisplayScaled(value);
		return FString::Printf(TEXT("%.1f%%"), constant);
	}
	
	const float HEALTH_TO_DISPLAY_FACTOR = 0.2f;
	float healthDisplayScaled(float health)
	{
		return health * HEALTH_TO_DISPLAY_FACTOR;
	}

	int healthAsDisplayAmount(float health)
	{
		const float scaled = healthDisplayScaled(health);
		return scaled < 1.0f ? FMath::CeilToInt(scaled) : FMath::RoundToInt(scaled);
	}

	float getPeriodForAnyDisplayHealthChange(float HealthPerSecond) {
		const float displayScaled = valueformat::healthDisplayScaled(HealthPerSecond);
		const float period = 1.0f / FMath::Min(1.0f, displayScaled);
		return period;
	}

	FText asCompactPlayerNumber(const int localPlayerIndex) {
		return getAsCompactPlayerNumberText(localPlayerIndex + 1);
	}


#define LOCTEXT_NAMESPACE "ValuesFormat"

	FText getAsWordOrNumber(int number) {
		switch (number) {
		case 0:
			return LOCTEXT("number_0", "zero");
		case 1:
			return LOCTEXT("number_1", "one");
		case 2:
			return LOCTEXT("number_2", "two");
		case 3:
			return LOCTEXT("number_3", "three");
		case 4:
			return LOCTEXT("number_4", "four");
		case 5:
			return LOCTEXT("number_5", "five");
		case 6:
			return LOCTEXT("number_6", "six");
		case 7:
			return LOCTEXT("number_7", "seven");
		case 8:
			return LOCTEXT("number_8", "eight");
		case 9:
			return LOCTEXT("number_9", "nine");
		default:
			return FText::FromString(FString::FromInt(number));
		}
	}

	//For usage in creating "Every [xth second] / [second]"
	FText getOrdinalEverySeconds(float value, const FText& countText) {
		const float seconds = FMath::Abs(value);
		if (seconds == 1.0f) {
			return FText::Format(LOCTEXT("every_ordinal_seconds_exactly_1", "second"), countText);
		}
		else {
			return FText::Format(LOCTEXT("every_ordinal_seconds_more_than_1", "{0} second"), countText);
		}
	}

	//For usage in creating "Every [x seconds]"
	FText getEverySeconds(float value, const FText& countText) {
		const float seconds = FMath::Abs(value);
		if (seconds == 1.0f) {
			return FText::Format(LOCTEXT("every_seconds_exactly_1", "second"), countText);
		} else {
			return FText::Format(LOCTEXT("every_seconds_more_than_1", "{0} seconds"), countText);
		}
	}

	//For usage in creating "...for [x seconds]" (same as getEverySeconds, but INCLUDING the ordinal for formatting)
	FText getForSeconds(float value, const FText& countText) {
		const int seconds = FMath::RoundToInt(FMath::Abs(value));
		if (seconds == 1) {
			return FText::Format(LOCTEXT("duration_for_seconds_exactly_1", "{0} second"), countText);
		}
		else {
			return FText::Format(LOCTEXT("duration_for_seconds_more_than_1", "{0} seconds"), countText);
		}
	}

	//For usage in creating "[x second] duration"
	FText getDurationSeconds(float value, const FText& countText) {
		const int seconds = FMath::RoundToInt(FMath::Abs(value));
		if (seconds == 1) {
			return FText::Format(LOCTEXT("duration_seconds_exactly_1", "{0} second"), countText);
		} else {
			return FText::Format(LOCTEXT("duration_seconds_more_than_1", "{0} second"), countText);
		}
	}

	FText getOrdinal(int number) {
		switch (number) {
		case 0:
			return LOCTEXT("ordinal_0", "0th");
		case 1:
			return LOCTEXT("ordinal_1", "1st");
		case 2:
			return LOCTEXT("ordinal_2", "2nd");
		case 3:
			return LOCTEXT("ordinal_3", "3rd");
		case 4:
			return LOCTEXT("ordinal_4", "4th");
		case 5:
			return LOCTEXT("ordinal_5", "5th");
		case 6:
			return LOCTEXT("ordinal_6", "6th");
		case 7:
			return LOCTEXT("ordinal_7", "7th");
		case 8:
			return LOCTEXT("ordinal_8", "8th");
		case 9:
			return LOCTEXT("ordinal_9", "9th");
		case 10:
			return LOCTEXT("ordinal_10", "10th");
		case 11:
			return LOCTEXT("ordinal_11", "11th");
		case 12:
			return LOCTEXT("ordinal_12", "12th");
		case 13:
			return LOCTEXT("ordinal_13", "13th");
		case 14:
			return LOCTEXT("ordinal_14", "14th");
		case 15:
			return LOCTEXT("ordinal_15", "15th");
		case 16:
			return LOCTEXT("ordinal_16", "16th");
		case 17:
			return LOCTEXT("ordinal_17", "17th");
		case 18:
			return LOCTEXT("ordinal_18", "18th");
		case 19:
			return LOCTEXT("ordinal_19", "19th");
		case 20:
			return LOCTEXT("ordinal_20", "20th");
		default:
			return FText::Format(LOCTEXT("ordinal_above_20", "{0}th"), FText::FromString(FString::FromInt(number)));
		}
	}

	FText getRoundedFractionalQuarters(float value){
		const int partsOfFour = FMath::RoundToInt(value * 4.0f);
		switch (partsOfFour) {
		case 0:
			return LOCTEXT("fraction_0_4", "zero");
		case 1:
			return LOCTEXT("fraction_1_4", "a quarter");
		case 2:
			return LOCTEXT("fraction_2_4", "half");
		case 3:
			return LOCTEXT("fraction_3_4", "three quarters");
		case 4:
			return LOCTEXT("fraction_4_4", "full");
		default:
			return FText::FromString(asPercentage(value));
		}
	}

	//For usage in creating "Every [x]th"
	FText getEveryOrdinal(int value) {
		const int valueabs = FMath::Abs(value);
		if (valueabs == 1) {
			return FText::Format(LOCTEXT("every_ordinal_exactly_1", ""), getOrdinal(valueabs));
		} else {
			return FText::Format(LOCTEXT("every_ordinal_more_than_1", " {0}"), getOrdinal(valueabs));
		}
	}

	FText getMultipleNoun(int value){
		switch (value) {		
		case 1:
			return LOCTEXT("multiplenoun_1", "single");
		case 2:
			return LOCTEXT("multiplenoun_2", "double");
		case 3:
			return LOCTEXT("multiplenoun_3", "triple");
		case 4:
			return LOCTEXT("multiplenoun_4", "quadruple");
		case 5:
			return LOCTEXT("multiplenoun_5", "quintuple");
		case 6:
			return LOCTEXT("multiplenoun_6", "sextuple");
		case 7:
			return LOCTEXT("multiplenoun_7", "septuple");
		case 8:
			return LOCTEXT("multiplenoun_8", "octuple");
		case 9:
			return LOCTEXT("multiplenoun_9", "nonuple");
		case 10:
			return LOCTEXT("multiplenoun_10", "decuple");
		default:
			return FText::FromString(asPercentage(value));
		}
	}

	DUNGEONS_API extern FText getAsCompactPlayerNumberText(int playernumber){				
		return FText::Format(LOCTEXT("compact_player_Px_Template", "P{0}"), FText::FromString(FString::FromInt(playernumber)));		
	}
	
	DUNGEONS_API extern FText asCounterLabeled(const FText& label, int current, int target) {
		return FText::FormatNamed(LOCTEXT("labeled_counter_with_target", "{current} / {target} {label}"), TEXT("label"), label, TEXT("current"), FText::FromString(FString::FromInt(current)), TEXT("target"), FText::FromString(FString::FromInt(target)));
	}

	DUNGEONS_API extern FText asCounterLabeled(const FText& label, int current)
	{
		return FText::FormatNamed(INVTEXT("{current} {label}"), TEXT("label"), label, TEXT("current"), FText::FromString(FString::FromInt(current)));
	}

	DUNGEONS_API extern FText asCounter(int current, int target) {
		return FText::FormatNamed(LOCTEXT("counter_with_target", "{current} / {target}"), TEXT("current"), FText::FromString(FString::FromInt(current)), TEXT("target"), FText::FromString(FString::FromInt(target)));
	}

	FString JoinDelimitedStringList(const TArray<FString>& strings, const FText& delimiter, const FText& finalDelimiter) {
		FString FormatString;
		for (int i = 0; i < strings.Num(); i++) {
			if (i > 0) {
				// force adding spaces for string concatenation, since it's too cumbersome to ensure proper formatting for all locales.
				FormatString += (i < strings.Num()-1 ? delimiter.ToString() : " "+finalDelimiter.ToString().TrimStartAndEnd()+" ");
			}
			FormatString += strings[i];
		}
		return FormatString;
	}

	DUNGEONS_API extern FText asDelimiterSeparatedList(const TArray<FText>& texts, const FText& delimiter, const FText& finalDelimiter) {
		auto increasingNumbers = algo::iota_tarray<int32>(texts.Num());
		TArray<FString> format = algo::map_as<TArray<FString>>(increasingNumbers, RETLAMBDA(FText::Format(FText::FromString("`{{0}}"), FText::FromString(FString::FromInt(it))).ToString()));
		return FText::Format(FText::FromString(JoinDelimitedStringList(format, delimiter, finalDelimiter)), algo::map_as<FFormatOrderedArguments>(texts, RETLAMBDA(FFormatArgumentValue( it ))));
	}

	DUNGEONS_API extern FText asCommaSeparatedList(const TArray<FText>& texts) {
		return asDelimiterSeparatedList(texts, LOCTEXT("commaseparatedlist_delimiter", ", "), LOCTEXT("commaseparatedlist_finaldelimiter", ", "));
	}

	DUNGEONS_API extern FText asCommaSeparatedOrList(const TArray<FText>& texts) {
		return asDelimiterSeparatedList(texts, LOCTEXT("commaseparatedlist_ordelimiter", ", "), LOCTEXT("commaseparatedlist_finalordelimiter", " or "));
	}

	DUNGEONS_API extern FText asCommaSeparatedAndList(const TArray<FText>& texts) {
		return asDelimiterSeparatedList(texts, LOCTEXT("commaseparatedlist_anddelimiter", ", "), LOCTEXT("commaseparatedlist_finalanddelimiter", " and "));
	}

#undef LOCTEXT_NAMESPACE
}