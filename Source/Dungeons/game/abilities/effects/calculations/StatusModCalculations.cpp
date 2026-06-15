#include "StatusModCalculations.h"
#include "Dungeons.h"
#include "game/abilities/attributes/StatusAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"

// ----- Helper Functions ----- //
namespace statuseffect {
	EStatusEffectType GetStatusEffectType(FGameplayTagContainer specTags) {

		if (specTags.HasTag(FGameplayTag::RequestGameplayTag("StatusEffect.Positive"))) {
			return EStatusEffectType::POSITIVE;
		}
		else if (specTags.HasTag(FGameplayTag::RequestGameplayTag("StatusEffect.Negative"))) {
			return EStatusEffectType::NEGATIVE;
		}

		return EStatusEffectType::UNTAGGED;
	}
}

// ----- Generic Calculation ----- //

UStatusModCalculation::UStatusModCalculation() {
	bAllowNonNetAuthorityDependencyRegistration = false;
}

// ----- Tag Determined Duration Calculation ----- //

UStatusDurationModCalculation::UStatusDurationModCalculation()
	:	PositiveDurationCapture(UStatusAttributeSet::PositiveStatusDurationMagnitudeAttribute(), EGameplayEffectAttributeCaptureSource::Target, false),
		NegativeDurationCapture(UStatusAttributeSet::NegativeStatusDurationMagnitudeAttribute(), EGameplayEffectAttributeCaptureSource::Target, false) {

	RelevantAttributesToCapture.Emplace(PositiveDurationCapture);
	RelevantAttributesToCapture.Emplace(NegativeDurationCapture);
}

float UStatusDurationModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	float durationMagnitude = Spec.GetSetByCallerMagnitude(effects::DurationName, false, 1);
	float multiplier = 1.f;

	FGameplayTagContainer assetTags;
	Spec.GetAllAssetTags(assetTags);

	switch (statuseffect::GetStatusEffectType(assetTags)) {
		case EStatusEffectType::POSITIVE:
			GetCapturedAttributeMagnitude(PositiveDurationCapture, Spec, FAggregatorEvaluateParameters(), multiplier);
			break;
		case EStatusEffectType::NEGATIVE:
			GetCapturedAttributeMagnitude(NegativeDurationCapture, Spec, FAggregatorEvaluateParameters(), multiplier);
			break;
		default:
			break;
	}

	return multiplier * durationMagnitude;
}
