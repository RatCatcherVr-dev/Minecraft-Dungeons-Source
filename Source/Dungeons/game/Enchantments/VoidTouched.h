// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffect.h>
#include "GameplayEffectExecutionCalculation.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "game/component/blocktriggers/VoidLiquidBlockTrigger.h"
#include "VoidTouched.generated.h"

UCLASS()
class DUNGEONS_API UVoidTouched : public UEnchantment
{
	GENERATED_BODY()
public:
	UVoidTouched();

	FText CreateDescription() const override;
protected:
	void AfterDealtDamage(AActor* toWhat);

	float maxTime = 5.0f;
	float timeToMax = 4.0f;
	float baseMagnitude = 0.25f;
};

UCLASS()
class DUNGEONS_API UVoidTouchedMelee : public UVoidTouched
{
	GENERATED_BODY()
public:
	UVoidTouchedMelee();

public:
	void OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext window) override;
};

UCLASS()
class DUNGEONS_API UVoidTouchedRanged : public UVoidTouched
{
	GENERATED_BODY()
	UVoidTouchedRanged();

public:
	void OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) override;
};

UCLASS()
class DUNGEONS_API UVoidStrikeMagnitudeExtenderGameplayEffect : public UVoidLiquidMagnitudeExtenderGameplayEffect
{
	GENERATED_BODY()
public:
	UVoidStrikeMagnitudeExtenderGameplayEffect();
};


UCLASS()
class DUNGEONS_API UVoidStrikeMagnitudeExtenderExecution : public UVoidLiquidMagnitudeExtenderExecution
{
	GENERATED_BODY()
public:
	UVoidStrikeMagnitudeExtenderExecution();
};

UCLASS()
class DUNGEONS_API UVoidStrikeGameplayEffect : public UVoidLiquidGameplayEffect
{
	GENERATED_BODY()
public:
	UVoidStrikeGameplayEffect();
};

UCLASS()
class DUNGEONS_API UVoidStrikeImmunityGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UVoidStrikeImmunityGameplayEffect();
};

UCLASS()
class DUNGEONS_API UVoidStrikePinnacleTimeGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UVoidStrikePinnacleTimeGameplayEffect();

};