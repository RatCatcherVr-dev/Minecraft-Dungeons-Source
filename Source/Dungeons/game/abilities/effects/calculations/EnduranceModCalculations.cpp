#include "Dungeons.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "EnduranceModCalculations.h"


URecoverEndurancePerSecondModCalculation::URecoverEndurancePerSecondModCalculation()
	: EnduranceRecoveryPerSecondAttributeCapture(UHealthAttributeSet::EnduranceRecoveryPerSecondAttribute(), EGameplayEffectAttributeCaptureSource::Source, true)
{
	bAllowNonNetAuthorityDependencyRegistration = false;
	RelevantAttributesToCapture.Emplace(EnduranceRecoveryPerSecondAttributeCapture);
}

float URecoverEndurancePerSecondModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	float enduranceRecoverPerSecond = 0.f;
	GetCapturedAttributeMagnitude(EnduranceRecoveryPerSecondAttributeCapture, Spec, FAggregatorEvaluateParameters(), enduranceRecoverPerSecond);
	return enduranceRecoverPerSecond * Spec.Period;
}