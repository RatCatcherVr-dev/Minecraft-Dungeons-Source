#pragma once

#include "GameplayEffect.h"
#include "GameplayEffectExecutionCalculation.h"
#include "Drowning.generated.h"

UCLASS()
class DUNGEONS_API UOxygenLowEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UOxygenLowEffect(const FObjectInitializer& ObjectInitializer);
};


UCLASS()
class DUNGEONS_API UOxygenPartialReplenishEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UOxygenPartialReplenishEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UOxygenReplenishOnlyEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UOxygenReplenishOnlyEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UOxygenReplenishEffect : public UOxygenReplenishOnlyEffect {
	GENERATED_BODY()
public:
	UOxygenReplenishEffect(const FObjectInitializer& ObjectInitializer);
};


UCLASS()
class DUNGEONS_API UHoldingBreathGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UHoldingBreathGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UHoldingBreathExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UHoldingBreathExecution();

	void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

protected:
	FGameplayAttribute Attribute;
	const FGameplayEffectAttributeCaptureDefinition OxygenCapture;
};

UCLASS()
class DUNGEONS_API UDrowningGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UDrowningGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UDrowningUIGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UDrowningUIGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UDrowningExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UDrowningExecution();

	void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	float PercentageDamagePerSecond = 5.0f;

protected:
	FGameplayAttribute Attribute;
	const FGameplayEffectAttributeCaptureDefinition OxygenCapture;
	const FGameplayEffectAttributeCaptureDefinition MaxHealthCapture;
	const FGameplayEffectAttributeCaptureDefinition EnvProtectionCapture;
};