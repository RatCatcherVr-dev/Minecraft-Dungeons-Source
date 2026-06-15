// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffect.h>
#include "FireAspect.generated.h"

UCLASS(BlueprintType, Blueprintable)
class DUNGEONS_API UFireAspectGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UFireAspectGameplayEffect();
};


UCLASS(BlueprintType, Blueprintable)
class DUNGEONS_API UFireAspect : public UEnchantment
{
	GENERATED_BODY()
	
public:
	UFireAspect();

	FText CreateDescription() const override;
	void OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext context) override;
	void OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UFireAspectGameplayEffect> Effect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	float MobDamagePerSecond = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	float DamagePerSecond = 30.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	float fireDuration = 3.f;

	void DebuffTarget(ABaseCharacter* target, FSharedPredictionContext context);
};


