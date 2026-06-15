#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include <GameplayEffect.h>
#include "LifestealExecution.generated.h"

UCLASS()
class DUNGEONS_API UBaseLifestealExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UBaseLifestealExecution();

	void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

protected:
	FGameplayAttribute Attribute;

	const FGameplayEffectAttributeCaptureDefinition TargetHealthCapture;
	FGameplayEffectAttributeCaptureDefinition LifeStealCapture;
};

UCLASS()
class DUNGEONS_API UMeleeLifestealExecution : public UBaseLifestealExecution
{
	GENERATED_BODY()
public:
	UMeleeLifestealExecution();
};

UCLASS()
class DUNGEONS_API URangedLifestealExecution : public UBaseLifestealExecution
{
	GENERATED_BODY()
public:
	URangedLifestealExecution();
};

UCLASS()
class DUNGEONS_API UItemLifestealExecution : public UBaseLifestealExecution
{
	GENERATED_BODY()
public:
	UItemLifestealExecution();
};

UCLASS()
class DUNGEONS_API ULifestealGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	ULifestealGameplayEffect();
};