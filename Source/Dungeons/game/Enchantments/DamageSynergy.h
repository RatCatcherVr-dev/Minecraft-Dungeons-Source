// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "Synergy.h"
#include "DamageSynergy.generated.h"


UCLASS()
class DUNGEONS_API UDamageSynergyGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UDamageSynergyGameplayEffect();

	static const FName DamageBonusKey;
};


UCLASS()
class DUNGEONS_API UDamageSynergy : public USynergy
{
	GENERATED_BODY()

public:
	UDamageSynergy();

	void OnBeforeDealtMeleeDamage(float& outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FSharedPredictionContext context);
	void OnAfterMeleeAttack(AActor* attackTarget, int32 hitCount, FVector attackVector, int32 index, FRandomStream& randStream, FSharedPredictionContext window);
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UDamageSynergyGameplayEffect> Effect;

	void OnItemSuccess() override;
	void OnEnd() override;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float BaseBonusDamagePercentage = 1.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float BonusDamagePercentagePerLevel = 0.2f;
	
private:
	FActiveGameplayEffectHandle EffectHandle;

	void RemovePendingBuff() const;
};
