// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffect.h>
#include "BowsBoon.generated.h"

UCLASS()
class DUNGEONS_API UBowsBoonGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UBowsBoonGameplayEffect(const FObjectInitializer& ObjectInitializer);
};


UCLASS()
class DUNGEONS_API UBowsBoon : public UEnchantment
{
	GENERATED_BODY()

public:
	UBowsBoon();

	void OnProjectileOverlap(AActor *what, AActor *whom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) override;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UBowsBoonGameplayEffect> Effect;
};
