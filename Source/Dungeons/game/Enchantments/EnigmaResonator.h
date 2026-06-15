// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "EnigmaResonator.generated.h"


UCLASS()
class DUNGEONS_API UEnigmaResonatorBase : public UEnchantment
{
	GENERATED_BODY()
public:
	UEnigmaResonatorBase();

	FText CreateDescription() const override;

protected:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag CriticalDamageType;

	UPROPERTY(EditDefaultsOnly)
	float DamageMultiplier = 3.0f;
};


UCLASS()
class DUNGEONS_API UEnigmaResonatorMelee : public UEnigmaResonatorBase
{
	GENERATED_BODY()
public:
	UEnigmaResonatorMelee();

	void OnBeforeDealtMeleeDamage(float &outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FSharedPredictionContext context);
};

UCLASS()
class DUNGEONS_API UEnigmaResonatorRanged : public UEnigmaResonatorBase
{
	GENERATED_BODY()
public:
	UEnigmaResonatorRanged();

	void OnBeforeDealtRangedDamage(float& outDamage, FGameplayTag& damageType, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) override;
};
