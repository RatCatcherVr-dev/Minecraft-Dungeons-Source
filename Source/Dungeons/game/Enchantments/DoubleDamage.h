// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "DoubleDamage.generated.h"

UCLASS()
class DUNGEONS_API UDoubleDamageGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UDoubleDamageGameplayEffect(const FObjectInitializer& ObjectInitializer);
};


UCLASS()
class DUNGEONS_API UDoubleDamage : public UEnchantment
{
	GENERATED_BODY()
	
public:
	UDoubleDamage();
protected:
	void OnStart() override;
	void OnEnd() override;
private:
	FActiveGameplayEffectHandle Handle;
};
