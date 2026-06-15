// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffect.h>
#include "Protecting.generated.h"


UCLASS()
class DUNGEONS_API UProtectingGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UProtectingGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UProtecting : public UEnchantment
{
	GENERATED_BODY()
	
	UProtecting();

	void OnStart() override;

	void OnEnd() override;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UProtectingGameplayEffect> Effect;

	FActiveGameplayEffectHandle Handle;
};
