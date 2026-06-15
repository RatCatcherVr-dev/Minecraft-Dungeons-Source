// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameplayEffect.h>
#include "ChainLightning.generated.h"

class ABaseCharacter;

UCLASS()
class DUNGEONS_API UChainLightningDamageGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UChainLightningDamageGameplayEffect();
};

UCLASS()
class DUNGEONS_API UMeleeChainLightningDamageGameplayEffect : public UChainLightningDamageGameplayEffect {
	GENERATED_BODY()
public:
	UMeleeChainLightningDamageGameplayEffect();
};


UCLASS()
class DUNGEONS_API URangedChainLightningDamageGameplayEffect : public UChainLightningDamageGameplayEffect {
	GENERATED_BODY()
public:
	URangedChainLightningDamageGameplayEffect();
};

namespace chainlightning {
	void ApplyChainLightning(TWeakObjectPtr<AActor> fromTarget, TArray<TWeakObjectPtr<ABaseCharacter>> targets, float ChainLightningDelay, float IndividualChainRadius, FGameplayEffectSpec spec);
}