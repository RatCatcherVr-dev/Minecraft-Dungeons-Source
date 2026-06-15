// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "PotionFortification.generated.h"

UCLASS()
class DUNGEONS_API UPotionFortificationGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UPotionFortificationGameplayEffect(const FObjectInitializer& ObjectInitializer);
};


UCLASS()
class DUNGEONS_API UPotionFortification : public UEnchantment
{
	GENERATED_BODY()
	
public:
	UPotionFortification();

	FText CreateDescription() const override;
protected:
	void OnStart() override;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float DamageAbsorbationIncrease = 10.0f;

private:
	UFUNCTION()
	void OnHealthPotionActivated(UItemSlot* slot, bool success);
	void ApplyEffect();

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<UPotionFortificationGameplayEffect> Effect;
};
