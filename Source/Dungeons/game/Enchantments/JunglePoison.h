// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffect.h>
#include "JunglePoison.generated.h"

UCLASS()
class DUNGEONS_API UJunglePoisonGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UJunglePoisonGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UJunglePoisonMelee : public UEnchantment
{
	GENERATED_BODY()
public:
	UJunglePoisonMelee();

	void OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext context) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class UJunglePoisonGameplayEffect> Effect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DamagePercentage = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Duration = 10.0f;
};

UCLASS()
class DUNGEONS_API UJunglePoisonRanged : public UEnchantment
{
	GENERATED_BODY()
public:
	UJunglePoisonRanged();

	void OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class UJunglePoisonGameplayEffect> Effect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DamagePercentage = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Duration = 10.0f;
};
