// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffectTypes.h>
#include "FastAttack.generated.h"

UCLASS()
class DUNGEONS_API UFastAttackGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UFastAttackGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UFastAttack : public UEnchantment
{
	GENERATED_BODY()
public:
	UFastAttack();
	void OnStart() override;
	void OnEnd() override;
private:
	FActiveGameplayEffectHandle Handle;
};
