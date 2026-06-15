// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "GameplayEffect.h"
#include "SlowImmunity.generated.h"

UCLASS()
class DUNGEONS_API USlowImmunityGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	USlowImmunityGameplayEffect();
};

UCLASS()
class DUNGEONS_API USlowImmunity : public UEnchantment
{
	GENERATED_BODY()
public:
	USlowImmunity();

	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type) override;

private:
	FActiveGameplayEffectHandle Handle;
};