// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "GameplayTagContainer.h"
#include <WeakObjectPtrTemplates.h>
#include "game/abilities/effects/StaggerGameplayEffect.h"
#include "Chains.generated.h"

class AMobCharacter;


UCLASS()
class DUNGEONS_API UChainsGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UChainsGameplayEffect();
};

 
UCLASS()
class DUNGEONS_API UChains : public UEnchantment
{
	GENERATED_BODY()
public:
	UChains();
	FText CreateDescription() const override;
protected:
	void OnBeforeMeleeAttack(AActor* attackTarget, FVector attackVector, int32 index, FRandomStream& randStream, FSharedPredictionContext window) override;

	void OnBeforeDealtMeleeDamage(float &outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FSharedPredictionContext window) override;
	void OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext window) override;
	
	// Increases with level e.g: BaseMobChainAMount + (Level - 1)
	UPROPERTY(EditDefaultsOnly)
	int BaseMobChainAmount = 2;
	// Time between chaining mobs
	UPROPERTY(EditDefaultsOnly)
	float ChainDelayTime = 0.1f;
	// Time mobs are chained
	UPROPERTY(EditDefaultsOnly)
	float BaseChainDuration = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	float MobChainDuration = 4.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float TriggerChance = 0.3f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float ChainRange = 1200.f;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UChainsGameplayEffect> Effect = UChainsGameplayEffect::StaticClass();
private:

	void OnChains(TWeakObjectPtr<ABaseCharacter> mob, const FGameplayTag tag, int32 tagCount);


	void SpawnChain(TWeakObjectPtr<ABaseCharacter> insitigator, TArray<TWeakObjectPtr<ABaseCharacter>> MobsToChain, float delay);
	bool CanBeChained(const ABaseCharacter* target);

	bool bCanTriggerChains = false;
};
