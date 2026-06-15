// © 2020 Mojang Synergies AB. TM Microsoft Corporation.


#include "TextFormatFunctionLibrary.h"


FText UTextFormatFunctionLibrary::TimeSpanAsPaddedCountdownSeconds(const FTimespan& timespan) {
	return FText::FromString(FString::Printf(TEXT("%02d:%02d:%02d:%02d"), timespan.GetDays(), timespan.GetHours(), timespan.GetMinutes(), timespan.GetSeconds()));
}

FText UTextFormatFunctionLibrary::DoublePaddedCountdown(int time)
{
	return FText::FromString(FString::Printf(TEXT("%02d"), time));
}
