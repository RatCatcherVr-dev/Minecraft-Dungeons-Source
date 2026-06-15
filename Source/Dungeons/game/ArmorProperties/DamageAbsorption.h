// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include "game/util/ValueFormat.h"
#include <GameplayEffect.h>
#include "DamageAbsorption.generated.h"

UCLASS()
class DUNGEONS_API UDamageAbsorptionGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UDamageAbsorptionGameplayEffect();
};

/**
 * 
 */
UCLASS()
class DUNGEONS_API UDamageAbsorption : public UArmorProperty
{
	GENERATED_BODY()
public:
	UDamageAbsorption();
	
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type) override;
	
protected:
	FString createFormattedValueString() const override { return valueformat::asMultiplierPercentageChange(Absorption); };

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Absorption = 0.9f;

private:
	FActiveGameplayEffectHandle Handle;
};
