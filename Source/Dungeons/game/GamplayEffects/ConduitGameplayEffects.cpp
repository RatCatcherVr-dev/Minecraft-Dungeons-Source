#include "ConduitGameplayEffects.h"
#include "Dungeons.h"

#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/util/DungeonsEffectLibrary.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include "util/CharacterQuery.h"

UMobDrowningGameplayEffect::UMobDrowningGameplayEffect(const FObjectInitializer& ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = FName(TEXT("Duration"));

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::NeverRefresh;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::NeverReset;
	DurationMagnitude = durationMagnitude;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;
	bSuppressStackingCues = true;
	Period = 1.0f;

	FAttributeBasedFloat healthMagnitude;
	healthMagnitude.AttributeCalculationType = EAttributeBasedFloatCalculationType::AttributeMagnitude;
	healthMagnitude.Coefficient = -PercentageDamagePerSecond / 100.f;
	healthMagnitude.BackingAttribute.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	healthMagnitude.BackingAttribute.AttributeToCapture = UHealthAttributeSet::MaxHealthAttribute();
	healthMagnitude.BackingAttribute.bSnapshot = false;

	FGameplayModifierInfo healthModifier;
	healthModifier.ModifierOp = EGameplayModOp::Additive;
	healthModifier.ModifierMagnitude = healthMagnitude;
	healthModifier.Attribute = UHealthAttributeSet::HealthAttribute();
	Modifiers.Add(healthModifier);

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Drowning"), 0, 1);
	InheritableGameplayEffectTags.AddTag(damageTag::damageFriends());
	InheritableGameplayEffectTags.AddTag(damageTag::drowning());
}

UConduitSpeedBoostGameplayEffect::UConduitSpeedBoostGameplayEffect(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FAttributeBasedFloat speedMagnitude;
	speedMagnitude.AttributeCalculationType = EAttributeBasedFloatCalculationType::AttributeMagnitude;
	speedMagnitude.Coefficient = SpeedMultiplier - 1;
	speedMagnitude.BackingAttribute.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	speedMagnitude.BackingAttribute.AttributeToCapture = UMovementAttributeSet::SpeedMultiplierAttribute();
	speedMagnitude.BackingAttribute.bSnapshot = false;

	FGameplayModifierInfo speedModifier;
	speedModifier.ModifierOp = EGameplayModOp::Additive;
	speedModifier.ModifierMagnitude = speedMagnitude;
	speedModifier.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();
	Modifiers.Add(speedModifier);
}