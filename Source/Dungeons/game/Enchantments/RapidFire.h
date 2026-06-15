// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffect.h>
#include "RapidFire.generated.h"



UCLASS()
class DUNGEONS_API URapidFireGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	URapidFireGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API URapidFire : public UEnchantment
{
	GENERATED_BODY()
public:
	URapidFire();
	void OnStart() override;
	void OnEnd() override;
private:
	FActiveGameplayEffectHandle Handle;
};
