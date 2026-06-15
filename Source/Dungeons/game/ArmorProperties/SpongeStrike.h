// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameplayEffect.h>
#include "game/component/MeleeAttackComponent.h"
#include "game/Enchantments/Enchantment.h"
#include "SpongeStrike.generated.h"

UCLASS()
class DUNGEONS_API USpongeStrikeDamageGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	USpongeStrikeDamageGameplayEffect();
};

UCLASS()
class DUNGEONS_API USpongeStrike : public UEnchantment
{
	GENERATED_BODY()
public:
	USpongeStrike();

	void OnAfterReceivedDamage(const struct FGameplayEffectModCallbackData &data, FRandomStream& randStream) override;

	void OnAfterMeleeAttack(AActor* attackTarget, int32 hitCount, FVector attackVector, int32 index, FRandomStream& randStream, FSharedPredictionContext window);

	void OnAfterResurrection() override;

private:
	void DoSpongeStrikeDamage(AActor* attackTarget);
	float DamageTaken = 0.f;
protected:
	void BeginPlay() override;

};