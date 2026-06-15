#include "Drowning.h"
#include "Dungeons.h"

#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/util/DungeonsEffectLibrary.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/attributes/OxygenAttributeSet.h"
#include "game/abilities/attributes/ResistanceAttributeSet.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include "game/abilities/effects/calculations/ResistanceModCalculations.h"
#include "util/CharacterQuery.h"

UOxygenLowEffect::UOxygenLowEffect(const FObjectInitializer& ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Oxygen.Low"), 0, 1);
}

UOxygenPartialReplenishEffect::UOxygenPartialReplenishEffect(const FObjectInitializer& ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FSetByCallerFloat oxygenMagnitude;
	oxygenMagnitude.DataName = TEXT("oxygenMagnitude");

	FGameplayModifierInfo oxygenModifier;
	oxygenModifier.ModifierOp = EGameplayModOp::Additive;
	oxygenModifier.ModifierMagnitude = oxygenMagnitude;
	oxygenModifier.Attribute = UOxygenAttributeSet::OxygenAttribute();
	Modifiers.Add(oxygenModifier);

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Oxygen.Replenish"), 0, 1);

	//Remove drowning when applied since we regain oxygen (holding breath needs to be applied separately with this effect, don't know why)
	RemoveGameplayEffectsWithTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Oxygen.Drowning"));
}

UOxygenReplenishOnlyEffect::UOxygenReplenishOnlyEffect(const FObjectInitializer& ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FAttributeBasedFloat oxygenMagnitude;
	oxygenMagnitude.AttributeCalculationType = EAttributeBasedFloatCalculationType::AttributeMagnitude;
	oxygenMagnitude.Coefficient = 1.f;
	oxygenMagnitude.BackingAttribute.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	oxygenMagnitude.BackingAttribute.AttributeToCapture = UOxygenAttributeSet::MaxOxygenAttribute();
	oxygenMagnitude.BackingAttribute.bSnapshot = true;

	FGameplayModifierInfo oxygenModifier;
	oxygenModifier.ModifierOp = EGameplayModOp::Override;
	oxygenModifier.ModifierMagnitude = oxygenMagnitude;
	oxygenModifier.Attribute = UOxygenAttributeSet::OxygenAttribute();
	Modifiers.Add(oxygenModifier);

	//Reapply Holding breath.
	FConditionalGameplayEffect holdingBreath;
	holdingBreath.EffectClass = UHoldingBreathGameplayEffect::StaticClass();
	ConditionalGameplayEffects.Add(holdingBreath);
}

UOxygenReplenishEffect::UOxygenReplenishEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Oxygen.Replenish"), 0, 1);
}

UHoldingBreathGameplayEffect::UHoldingBreathGameplayEffect(const FObjectInitializer& ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = FName(TEXT("Duration"));

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	DurationMagnitude = durationMagnitude;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;
	bSuppressStackingCues = true;
	Period = 0.25f;

	FGameplayModifierInfo oxygenModifier;
	oxygenModifier.ModifierOp = EGameplayModOp::Additive;
	oxygenModifier.ModifierMagnitude = FScalableFloat(-0.25f);
	oxygenModifier.Attribute = UOxygenAttributeSet::OxygenAttribute();
	Modifiers.Add(oxygenModifier);

	FGameplayEffectExecutionDefinition oxygenExecution;
	oxygenExecution.CalculationClass = UHoldingBreathExecution::StaticClass();
	Executions.Add(std::move(oxygenExecution));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.HoldingBreath"), 0, 1);
	const auto holdingEffectTag = FGameplayTag::RequestGameplayTag("StatusEffect.Oxygen.HoldingBreath");
	InheritableOwnedTagsContainer.AddTag(holdingEffectTag);
	InheritableGameplayEffectTags.AddTag(holdingEffectTag);
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("PersistAfterDeath"));

	OngoingTagRequirements.RequireTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Underwater"));
	OngoingTagRequirements.IgnoreTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.WaterBreathing"));

	//When we are holding our breath, we should stop drowning!
	RemoveGameplayEffectsWithTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Oxygen.Drowning"));
}

UHoldingBreathExecution::UHoldingBreathExecution()
	: OxygenCapture(UOxygenAttributeSet::OxygenAttribute(), EGameplayEffectAttributeCaptureSource::Target, false)
{
	RelevantAttributesToCapture.Emplace(OxygenCapture);
}

void UHoldingBreathExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	float currentOxygen = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(OxygenCapture, FAggregatorEvaluateParameters(), currentOxygen);

	if (currentOxygen <= 0.f)
	{
		auto abilitySystem = ExecutionParams.GetTargetAbilitySystemComponent();
		abilitySystem->ApplyGameplayEffectSpecToSelf(effects::CreateGameplayEffectSpec<UDrowningGameplayEffect>(abilitySystem));
	}
}

UDrowningGameplayEffect::UDrowningGameplayEffect(const FObjectInitializer& ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::NeverRefresh;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::NeverReset;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;
	bSuppressStackingCues = true;
	Period = 1.0f;

	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UEnvironmentalProtectionModCalculation::StaticClass();

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	const auto drowningDamageTag = damageTag::drowning();
	const auto environmentalDamageTag = damageTag::environmental();
	FGameplayEffectExecutionDefinition drowningExecution;
	drowningExecution.CalculationClass = UDrowningExecution::StaticClass();
	drowningExecution.PassedInTags.AddTag(drowningDamageTag);
	drowningExecution.PassedInTags.AddTag(environmentalDamageTag);
	Executions.Add(std::move(drowningExecution));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Drowning"), 0, 1);
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Damage.Medium"), 0, 1);
	const auto drowningEffectTag = FGameplayTag::RequestGameplayTag("StatusEffect.Oxygen.Drowning");
	InheritableOwnedTagsContainer.AddTag(drowningEffectTag);
	InheritableGameplayEffectTags.AddTag(drowningEffectTag);
	InheritableGameplayEffectTags.AddTag(drowningDamageTag);
	InheritableGameplayEffectTags.AddTag(environmentalDamageTag);
	InheritableGameplayEffectTags.AddTag(damageTag::damageFriends());

	//Pause effect if we are not underwater
	OngoingTagRequirements.RequireTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Underwater"));

	//When we are drowning, we should stop holding our breath
	RemoveGameplayEffectsWithTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Oxygen.HoldingBreath"));
}

UDrowningExecution::UDrowningExecution() 
	: OxygenCapture(UOxygenAttributeSet::OxygenAttribute(), EGameplayEffectAttributeCaptureSource::Target, false)
	, MaxHealthCapture(UHealthAttributeSet::MaxHealthAttribute(), EGameplayEffectAttributeCaptureSource::Target, false)
	, EnvProtectionCapture(UResistanceAttributeSet::EnvironmentalProtectionMagnitudeAttribute(), EGameplayEffectAttributeCaptureSource::Target, false)
{
	RelevantAttributesToCapture.Emplace(OxygenCapture);
	RelevantAttributesToCapture.Emplace(MaxHealthCapture);
	RelevantAttributesToCapture.Emplace(EnvProtectionCapture);
}

void UDrowningExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	float currentOxygen = 0.f;
	float currentMaxHealth = 0.f;
	float protectionMagnitude = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(OxygenCapture, FAggregatorEvaluateParameters(), currentOxygen);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(MaxHealthCapture, FAggregatorEvaluateParameters(), currentMaxHealth);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(EnvProtectionCapture, FAggregatorEvaluateParameters(), protectionMagnitude);

	if (currentOxygen > 0.f)
	{
		auto abilitySystem = ExecutionParams.GetTargetAbilitySystemComponent();
		FGameplayEffectSpec holdingBreathSpec(Cast<UHoldingBreathGameplayEffect>(UHoldingBreathGameplayEffect::StaticClass()->GetDefaultObject()), abilitySystem->MakeEffectContext(), 1);
		abilitySystem->ApplyGameplayEffectSpecToSelf(holdingBreathSpec);
	}
	else
	{
		float damage = (-PercentageDamagePerSecond/100.f * ExecutionParams.GetOwningSpec().GetPeriod() * currentMaxHealth) * protectionMagnitude;
		FGameplayModifierEvaluatedData healthSubtraction(UHealthAttributeSet::HealthAttribute(), EGameplayModOp::Additive, damage);
		OutExecutionOutput.AddOutputModifier(healthSubtraction);
	}
}

UDrowningUIGameplayEffect::UDrowningUIGameplayEffect(const FObjectInitializer& ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::NeverRefresh;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::NeverReset;
	bSuppressStackingCues = true;

	const auto drowningEffectTag = FGameplayTag::RequestGameplayTag("StatusEffect.Oxygen.Drowning");
	InheritableOwnedTagsContainer.AddTag(drowningEffectTag);
	InheritableGameplayEffectTags.AddTag(drowningEffectTag);
}