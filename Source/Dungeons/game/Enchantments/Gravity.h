// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "Gravity.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UGravity : public UEnchantment
{
	GENERATED_BODY()
	
public:
	UGravity();

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	void OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) override;
protected:

	void StartPull(const FVector& location, bool executeCue = true);

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float PullTime = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float PullRadius = 500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float MobPullTime = 3.0f;

	UFUNCTION()
	void OnStopPull();

	FVector PullLocation;
	
	TArray<TWeakObjectPtr<class ABaseCharacter>> TargetsToPull;
};

UCLASS()
class DUNGEONS_API UGravityMelee : public UGravity {

	GENERATED_BODY()

public:
	UGravityMelee();

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	void OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) override;

	void OnBeforeMeleeAttack(AActor* attackTarget, FVector attackVector, int32 index, FRandomStream& randStream, FSharedPredictionContext window) override;
	void OnBeforeDealtMeleeDamage(float &outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FSharedPredictionContext window) override;
	void OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext window) override;
private:

	bool CanPull = false;
};

