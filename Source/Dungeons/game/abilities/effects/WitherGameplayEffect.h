#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "executions/DamageExecutionCalculation.h"
#include <GameplayEffectTypes.h>
#include "calculations/DamageModCalculations.h"
#include "WitherGameplayEffect.generated.h"

class UHealthComponent;

namespace wither {
	constexpr float PERIOD = 0.25f;

	float getWitherDurationForFraction(float fraction);
	float getWitherFractionForDuration(float duration);

	float clampWitherDuration(float healthPercentage, float duration);
	float getDurationLeft(const FActiveGameplayEffect*, UWorld&);
	FActiveGameplayEffectHandle getActiveEffectHandle(UAbilitySystemComponent*);
	const FActiveGameplayEffect* getActiveEffect(UAbilitySystemComponent*, const FActiveGameplayEffectHandle&);
	const FActiveGameplayEffect* getActiveEffect(UAbilitySystemComponent*);
	void setDurationForEffect(UAbilitySystemComponent*, const FActiveGameplayEffectHandle&, float);
}

UCLASS()
class DUNGEONS_API UWitherApplierExecutionCalculation : public UGameplayEffectExecutionCalculation {
	GENERATED_BODY()
public:
	UWitherApplierExecutionCalculation();
	void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UWitherGameplayEffect> WitherEffectClass;
private:
	float GetHealthPercentage(const FGameplayEffectSpec&) const;
	const FGameplayEffectAttributeCaptureDefinition HealthAttributeCapture;
	const FGameplayEffectAttributeCaptureDefinition MaxHealthAttributeCapture;
};

UCLASS()
class DUNGEONS_API UWitherDamageExecutionCalculation : public UDamageExecutionCalculation {
	GENERATED_BODY()
public:
	UWitherDamageExecutionCalculation();
	void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
private:
	float GetHealthPercentage(const FGameplayEffectSpec&) const;
	const FGameplayEffectAttributeCaptureDefinition HealthAttributeCapture;
	const FGameplayEffectAttributeCaptureDefinition MaxHealthAttributeCapture;
};

UCLASS()
class UWitherApplierGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UWitherApplierGameplayEffect();
};

UCLASS()
class UWitherGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UWitherGameplayEffect();
};
