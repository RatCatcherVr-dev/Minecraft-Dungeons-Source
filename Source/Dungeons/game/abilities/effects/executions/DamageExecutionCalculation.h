#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "DamageExecutionCalculation.generated.h"

class APlayerCharacter;

UCLASS()
class DUNGEONS_API UDamageExecutionCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:	
	void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

	static bool IsTargetAlive(const UAbilitySystemComponent* targetAbilitySystem, const FGameplayEffectModifiedAttribute* damageAttribute);

	APlayerCharacter* GetPlayerSource(const FGameplayEffectSpec& spec) const;

	void ExecuteDamageNumberGameplayCue(const FGameplayEffectSpec& spec, UAbilitySystemComponent* targetAbilitySystem,
	                                           const FGameplayEffectModifiedAttribute* damageAttribute, APlayerCharacter* playerSource) const;
};