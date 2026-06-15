// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "SpiritSpeed.generated.h"



UCLASS()
class DUNGEONS_API USpiritSpeedGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	USpiritSpeedGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API USpiritSpeed : public UEnchantment
{
	GENERATED_BODY()
public:
	USpiritSpeed();

	FText CreateDescription() const override;
protected:
	void OnStart() override;

	UPROPERTY(EditDefaultsOnly)
	float speedBonus = 0.05f;

private:
	UFUNCTION()
	void OnGatheredSoul();
	void BoostSpeed();

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<USpiritSpeedGameplayEffect> Effect;
};
