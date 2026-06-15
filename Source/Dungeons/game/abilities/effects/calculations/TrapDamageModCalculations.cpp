#include "Dungeons.h"
#include "TrapDamageModCalculations.h"
#include "game/abilities/attributes/ResistanceAttributeSet.h"
#include "DamageModCalculations.h"

UTrapDamageModCalculation::UTrapDamageModCalculation()
	: ResistanceCapture(UResistanceAttributeSet::EnvironmentalProtectionMagnitudeAttribute(), EGameplayEffectAttributeCaptureSource::Target, false)
{
	RelevantAttributesToCapture.Emplace(ResistanceCapture);
}


float UTrapDamageModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	float resistanceAppliedMultiplier = 1.f;
	GetCapturedAttributeMagnitude(ResistanceCapture, Spec, FAggregatorEvaluateParameters(), resistanceAppliedMultiplier);
	float damage = Super::CalculateBaseMagnitude_Implementation(Spec);
	return damage * resistanceAppliedMultiplier;
}
