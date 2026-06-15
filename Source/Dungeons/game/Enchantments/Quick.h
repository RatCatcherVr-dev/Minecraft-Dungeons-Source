// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "Quick.generated.h"

UCLASS()
class DUNGEONS_API UQuickGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UQuickGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UQuick : public UEnchantment
{
	GENERATED_BODY()
public:
	UQuick();
	void OnStart() override;
	void OnEnd() override;
private:
	FActiveGameplayEffectHandle Handle;
};
