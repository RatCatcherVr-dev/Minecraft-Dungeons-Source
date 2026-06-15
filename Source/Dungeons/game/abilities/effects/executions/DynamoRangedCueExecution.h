#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "DynamoRangedCueExecution.generated.h"


UCLASS()
class DUNGEONS_API UDynamoRangedCueExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:

	void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

};