// © 2020 Mojang Synergies AB. TM Microsoft Corporation.


#include "GameplayBlueprintFunctionLibrary.h"
#include "AIController.h"

FGameplayTagContainer UGameplayBlueprintFunctionLibrary::Filter(const FGameplayTagContainer& container, const FGameplayTagContainer& filter) {
	return container.Filter(filter);
}

void UGameplayBlueprintFunctionLibrary::SetMobsIgnorePlayers(const UObject* WorldContextObject, bool ignorePlayers)
{
	if (!WorldContextObject)
		return;
	
	AAIController::bAIIgnorePlayers = ignorePlayers;
}
