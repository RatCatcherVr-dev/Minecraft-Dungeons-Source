// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TextFormatFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UTextFormatFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText TimeSpanAsPaddedCountdownSeconds(const FTimespan& timespan);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText DoublePaddedCountdown(int time);
};
