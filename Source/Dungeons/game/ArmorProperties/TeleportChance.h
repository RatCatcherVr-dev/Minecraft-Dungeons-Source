// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include "game/util/ValueFormat.h"
#include "TeleportChance.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UTeleportChance : public UArmorProperty
{
	GENERATED_BODY()

public:
	UTeleportChance();

	void OnAfterReceivedDamage(const struct FGameplayEffectModCallbackData &data, FRandomStream& randStream) override;
	
protected:
	FString createFormattedValueString() const override { return valueformat::asPercentageChance(Chance);} ;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Chance = 0.1f;

	UPROPERTY(EditDefaultsOnly)
	float TeleportRange = 1000.0f;

};
