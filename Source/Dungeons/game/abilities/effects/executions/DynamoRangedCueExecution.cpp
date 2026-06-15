#include "DynamoRangedCueExecution.h"
#include "game/Enchantments/Dynamo.h"
#include <GameplayEffectExecutionCalculation.h>
#include <AbilitySystemComponent.h>


void UDynamoRangedCueExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const {
	int stacks = FMath::FloorToInt(ExecutionParams.GetOwningSpec().GetSetByCallerMagnitude(UDynamoRanged::DynamoRangedStackCountKey, false, 0.f));
	if (stacks > 0) {
		FGameplayCueParameters params;
		params.RawMagnitude = stacks;

		ExecutionParams.GetTargetAbilitySystemComponent()->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Hit.Dynamo.Ranged")), params);
	}
}