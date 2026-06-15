// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "GameplayBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UGameplayBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure, Category = "GameplayTags")
	static FGameplayTagContainer Filter(const FGameplayTagContainer& container, const FGameplayTagContainer& filter);

	UFUNCTION(BlueprintCallable, Category = "Dungeons", meta = (WorldContext = "WorldContextObject"))
	static void SetMobsIgnorePlayers(const UObject* WorldContextObject, bool ignorePlayers);
};
