#include "DodgeRechargingGameplayEffect.h"
#include "game/abilities/attributes/MovementAttributeSet.h"


const FName UDodgeRechargingGameplayEffect::DurationName = TEXT("Duration");
const FName UDodgeRechargingGameplayEffect::SpeedFactorName = TEXT("SpeedFactor");
const FName UDodgeRechargingGameplayEffect::FrictionFactorName = TEXT("FrictionFactor");
const FName UDodgeRechargingGameplayEffect::RotationFactorName = TEXT("RotationFactor");

UDodgeRechargingGameplayEffect::UDodgeRechargingGameplayEffect(const FObjectInitializer& ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FAttributeBasedFloat durationMagnitude;
	durationMagnitude.BackingAttribute.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	durationMagnitude.BackingAttribute.AttributeToCapture = UMovementAttributeSet::DodgeCooldownAttribute();

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	DurationMagnitude = durationMagnitude;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo speedFactorInfo;
	speedFactorInfo.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();

	FSetByCallerFloat speedMagnitude;
	speedMagnitude.DataName = SpeedFactorName;

	speedFactorInfo.ModifierMagnitude = speedMagnitude;
	speedFactorInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(speedFactorInfo);



	FGameplayModifierInfo frictionFactorInfo;
	frictionFactorInfo.Attribute = UMovementAttributeSet::FrictionMultiplierAttribute();

	FSetByCallerFloat frictionMagnitude;
	frictionMagnitude.DataName = FrictionFactorName;

	frictionFactorInfo.ModifierMagnitude = frictionMagnitude;
	frictionFactorInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(frictionFactorInfo);



	FGameplayModifierInfo rotationFactorInfo;
	rotationFactorInfo.Attribute = UMovementAttributeSet::RotationMultiplierAttribute();

	FSetByCallerFloat rotationMagnitude;
	rotationMagnitude.DataName = RotationFactorName;

	rotationFactorInfo.ModifierMagnitude = rotationMagnitude;
	rotationFactorInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(rotationFactorInfo);

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.DodgeRecharge"), 0, 1);		
}