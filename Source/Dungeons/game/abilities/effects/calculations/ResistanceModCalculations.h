#pragma once

#include <GameplayEffectExecutionCalculation.h>
#include <GameplayModMagnitudeCalculation.h>
#include <GameplayEffectTypes.h>
#include "DamageModCalculations.h"
#include "ResistanceModCalculations.generated.h"

// ----- Generic Resistance ----- //
UCLASS()
class DUNGEONS_API UResistanceModCalculation : public UGameplayModMagnitudeCalculation{
	GENERATED_BODY()
public:
	UResistanceModCalculation();
};

// ----- Slow Resistance ----- //
// Used to calculate resistances applied to gameplay effects that would slow the player in any way.
UCLASS()
class DUNGEONS_API USlowResistanceModCalculation : public UResistanceModCalculation {
	GENERATED_BODY()
public:
	USlowResistanceModCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
	//This only works because this class is never instantiated!
	const FGameplayEffectAttributeCaptureDefinition ResistanceCapture;
};

// ----- Freezing Slow Resistance ----- //
// Child of slow resistance mod calculation, specifically handles freezing resistance
UCLASS()
class DUNGEONS_API USlowMultiplicativeResistanceModCalculation : public USlowResistanceModCalculation {
	GENERATED_BODY()
public:
	static const FName SetByCallerKey;
	
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};


// ----- Environmental Protection ----- //
// Used to calculate resistances applied to gameplay effects that would damage the player when he takes damage from the world.
UCLASS()
class DUNGEONS_API UEnvironmentalProtectionModCalculation : public UWorldDamageModCalculation {
	GENERATED_BODY()
public:
	UEnvironmentalProtectionModCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
	//This only works because this class is never instantiated!
	const FGameplayEffectAttributeCaptureDefinition ResistanceCapture;
};


// ----- Fall Resistance ----- //
// Used to calculate resistances applied to gameplay effects that would damage the player when he felt.
UCLASS()
class DUNGEONS_API UFallResistanceModCalculation : public UResistanceModCalculation {
	GENERATED_BODY()
public:
	UFallResistanceModCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
	//This only works because this class is never instantiated!
	const FGameplayEffectAttributeCaptureDefinition ResistanceCapture;
};
