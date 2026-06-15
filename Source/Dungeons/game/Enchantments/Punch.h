// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "GameplayEffect.h"
#include "Punch.generated.h"

UCLASS()
class DUNGEONS_API UPunchGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UPunchGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

/**
*
*/
UCLASS()
class DUNGEONS_API UPunch : public UEnchantment
{
	GENERATED_BODY()
public:
	UPunch();

	void OnStart() override;

	void OnEnd() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UPunchGameplayEffect> Effect;

	FActiveGameplayEffectHandle Handle;
};

