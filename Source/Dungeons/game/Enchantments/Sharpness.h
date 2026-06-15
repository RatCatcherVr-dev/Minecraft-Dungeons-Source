// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "GameplayEffect.h"
#include "Sharpness.generated.h"

UCLASS()
class DUNGEONS_API USharpnessGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	USharpnessGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class DUNGEONS_API USharpness : public UEnchantment
{
	GENERATED_BODY()

	USharpness();

	void OnStart() override;

	void OnEnd() override;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<USharpnessGameplayEffect> Effect;

	FActiveGameplayEffectHandle Handle;
};
