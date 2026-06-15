#include "HealthDrain.h"
#include "game/abilities/attributes/HealthAttributeSet.h"

UPercentageHealthDrainGameplayEffect::UPercentageHealthDrainGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::NeverRefresh;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::NeverReset;
	bExecutePeriodicEffectOnApplication = false;
	Period = 1.0f;

	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UPercentageHealthDrainModCalculation::StaticClass();

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;

	Modifiers.Add(healthInfo);
}

UPercentageHealthDrainModCalculation::UPercentageHealthDrainModCalculation()
	: HealthCapture(UHealthAttributeSet::HealthAttribute(), EGameplayEffectAttributeCaptureSource::Target, false)
	, MaxHealthCapture(UHealthAttributeSet::MaxHealthAttribute(), EGameplayEffectAttributeCaptureSource::Target, false)
{
	RelevantAttributesToCapture.Emplace(HealthCapture);
	RelevantAttributesToCapture.Emplace(MaxHealthCapture);
}

float UPercentageHealthDrainModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec & Spec) const
{
	float currentHealth = 0.f;
	float currentMaxHealth = 0.f;

	const FAggregatorEvaluateParameters params;
	GetCapturedAttributeMagnitude(HealthCapture, Spec, params, currentHealth);
	GetCapturedAttributeMagnitude(MaxHealthCapture, Spec, params, currentMaxHealth);
	
	const float drain = currentMaxHealth * PercentageDrain * 0.01f;
	return -FMath::Min(currentHealth, drain);
}