#include "Dungeons.h"
#include "game/abilities/attributes/ResistanceAttributeSet.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "ResistanceModCalculations.h"

// Some helper functions
namespace resistances {
	float CalculateResistancesMultiplicative(float desiredMagnitude, float resistance) {
		if (desiredMagnitude >= 1) {
			return 1 + ((desiredMagnitude - 1) * resistance);
		}

		return desiredMagnitude * resistance;
	}
}

// ----- Generic Resistance ----- //
// Use this as a base so we can inherit any functions we may need.

UResistanceModCalculation::UResistanceModCalculation() {
	bAllowNonNetAuthorityDependencyRegistration = false;
}

// ----- Slow Resistance Mod Calculation ----- //

USlowResistanceModCalculation::USlowResistanceModCalculation()
	: ResistanceCapture(UResistanceAttributeSet::SlowResistanceMagnitudeAttribute(), EGameplayEffectAttributeCaptureSource::Target, false)
{
	RelevantAttributesToCapture.Emplace(ResistanceCapture);
}

float USlowResistanceModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	float resistanceAppliedMultiplier = 1.f;
	GetCapturedAttributeMagnitude(ResistanceCapture, Spec, FAggregatorEvaluateParameters(), resistanceAppliedMultiplier);
	return resistanceAppliedMultiplier;
}


const FName USlowMultiplicativeResistanceModCalculation::SetByCallerKey(TEXT("SlowMultiplicativeKey"));

float USlowMultiplicativeResistanceModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	float desiredMagnitude = Spec.GetSetByCallerMagnitude(SetByCallerKey, false, 1);
	float resistanceMultiplier = Super::CalculateBaseMagnitude_Implementation(Spec);
	return resistances::CalculateResistancesMultiplicative(desiredMagnitude, resistanceMultiplier);
}


// ----- Environmental Protection Mod Calculation ----- //

UEnvironmentalProtectionModCalculation::UEnvironmentalProtectionModCalculation()
	: ResistanceCapture(UResistanceAttributeSet::EnvironmentalProtectionMagnitudeAttribute(), EGameplayEffectAttributeCaptureSource::Target, false) 
{
	RelevantAttributesToCapture.Emplace(ResistanceCapture);
}

float UEnvironmentalProtectionModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	float resistanceAppliedMultiplier = 1.f;
	GetCapturedAttributeMagnitude(ResistanceCapture, Spec, FAggregatorEvaluateParameters(), resistanceAppliedMultiplier);
	float damage = Spec.GetSetByCallerMagnitude("Health");
	if (damage == 0)
	{
		damage = Super::CalculateBaseMagnitude_Implementation(Spec);
	}
	return damage * resistanceAppliedMultiplier;
}
// ----- Fall Resistance Mod Calculation ----- //

UFallResistanceModCalculation::UFallResistanceModCalculation()
	: ResistanceCapture(UResistanceAttributeSet::FallResistanceMagnitudeAttribute(), EGameplayEffectAttributeCaptureSource::Target, false) 
{
	RelevantAttributesToCapture.Emplace(ResistanceCapture);
}

float UFallResistanceModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	float resistanceAppliedMultiplier = 1.f;
	GetCapturedAttributeMagnitude(ResistanceCapture, Spec, FAggregatorEvaluateParameters(), resistanceAppliedMultiplier);
	const float damage = Spec.GetSetByCallerMagnitude("Health");
	return damage * resistanceAppliedMultiplier;
}
