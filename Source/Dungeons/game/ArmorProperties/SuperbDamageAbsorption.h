// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include "game/util/ValueFormat.h"
#include <GameplayEffect.h>
#include "SuperbDamageAbsorption.generated.h"

UCLASS()
class DUNGEONS_API USuperbDamageAbsorptionGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	USuperbDamageAbsorptionGameplayEffect();
};

/**
 * 
 */
UCLASS()
class DUNGEONS_API USuperbDamageAbsorption : public UArmorProperty
{
	GENERATED_BODY()
public:
	USuperbDamageAbsorption();
	
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type) override;
	
protected:
	FString createFormattedValueString() const override { return valueformat::asMultiplierPercentageChange(Absorbation); };

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Absorbation = 0.65f;

private:
	FActiveGameplayEffectHandle Handle;
};
