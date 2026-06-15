#include "WitherGameplayEffect.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "calculations/DamageModCalculations.h"
#include "executions/DamageExecutionCalculation.h"
#include "GameplayEffectUtil.h"
#include "game/actor/character/BaseCharacter.h"
#include <AbilitySystemComponent.h>
#include "game/component/HealthComponent.h"
#include "util/ClassUtil.h"
#include "calculations/StatusModCalculations.h"

namespace wither {
constexpr float FRACTION_PER_SECOND = 0.05f;
constexpr float DAMAGE_PER_TICK = FRACTION_PER_SECOND * PERIOD;

float getWitherDurationForFraction(float fraction) {
	constexpr float multiplier = 1000.f;

	return FMath::FloorToFloat((fraction * multiplier) / (DAMAGE_PER_TICK * multiplier)) * PERIOD;
}

float getWitherFractionForDuration(float duration) {
	return duration * FRACTION_PER_SECOND;
}

float clampWitherDuration(float healthPercentage, float duration) {
	const auto maxHealthFraction = healthPercentage - 0.01f;
	const auto maxDuration = getWitherDurationForFraction(maxHealthFraction);
	return FMath::Clamp(duration, 0.f, maxDuration);
}

float getDurationLeft(const FActiveGameplayEffect* effect, UWorld& world) {
	if (effect) {
		return effect->GetTimeRemaining(UGameplayStatics::GetTimeSeconds(&world));
	}
	return 0;
}

FActiveGameplayEffectHandle getActiveEffectHandle(UAbilitySystemComponent* abilitySystem) {
	static FGameplayEffectQuery query = FGameplayEffectQuery::MakeQuery_MatchAllOwningTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Wither"))));
	auto effects = abilitySystem->GetActiveEffects(query);
	return effects.Num() > 0 ? effects[0] : FActiveGameplayEffectHandle();
}

const FActiveGameplayEffect* getActiveEffect(UAbilitySystemComponent* abilitySystem, const FActiveGameplayEffectHandle& handle) {
	if (handle.IsValid()) {
		return abilitySystem->GetActiveGameplayEffect(handle);
	}
	return nullptr;
}

const FActiveGameplayEffect* getActiveEffect(UAbilitySystemComponent* abilitySystem) {
	auto handle = getActiveEffectHandle(abilitySystem);
	if (handle.IsValid()) {
		return abilitySystem->GetActiveGameplayEffect(handle);
	}
	return nullptr;
}

void setDurationForEffect(UAbilitySystemComponent* abilitySystem, const FActiveGameplayEffectHandle& handle, float duration) {
	auto activeEffect = getActiveEffect(abilitySystem, handle);
	const auto durationLeft = getDurationLeft(activeEffect, *abilitySystem->GetWorld());
	const auto durationDelta = duration - durationLeft;
	if (!FMath::IsNearlyZero(durationDelta) && durationLeft > 0.f) {
		abilitySystem->ModifyActiveEffectStartTime(handle, durationDelta);
	}
}

}

UWitherApplierExecutionCalculation::UWitherApplierExecutionCalculation()
	: HealthAttributeCapture(UHealthAttributeSet::HealthAttribute(), EGameplayEffectAttributeCaptureSource::Target, false)
	, MaxHealthAttributeCapture(UHealthAttributeSet::MaxHealthAttribute(), EGameplayEffectAttributeCaptureSource::Target, false) {
	RelevantAttributesToCapture.Emplace(HealthAttributeCapture);
	RelevantAttributesToCapture.Emplace(MaxHealthAttributeCapture);
}

void UWitherApplierExecutionCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const {
	auto abilitySystem = ExecutionParams.GetTargetAbilitySystemComponent();
	auto applySpec = ExecutionParams.GetOwningSpec();
	const auto durationLeft = wither::getDurationLeft(wither::getActiveEffect(abilitySystem), *abilitySystem->GetWorld());
	const auto witherDuration = applySpec.GetSetByCallerMagnitude(effects::DurationName);
	const auto clampedDuration = wither::clampWitherDuration(GetHealthPercentage(applySpec), durationLeft + witherDuration);
	auto witherSpec = effects::CreateGameplayEffectSpecFromSubClass(abilitySystem, WitherEffectClass);
	witherSpec.SetSetByCallerMagnitude(effects::DurationName, clampedDuration);
	witherSpec.SetDuration(clampedDuration, false);
	witherSpec.SetContext(applySpec.GetContext());
	abilitySystem->ApplyGameplayEffectSpecToSelf(witherSpec);
}

float UWitherApplierExecutionCalculation::GetHealthPercentage(const FGameplayEffectSpec& spec) const {
	float health = 1.f;
	if (const FGameplayEffectAttributeCaptureSpec* healthSpec = spec.CapturedRelevantAttributes.FindCaptureSpecByDefinition(HealthAttributeCapture, true)) {
		healthSpec->AttemptCalculateAttributeMagnitude(FAggregatorEvaluateParameters(), health);
	}
	float maxHealth = 1.f;
	if (const FGameplayEffectAttributeCaptureSpec* maxHealthSpec = spec.CapturedRelevantAttributes.FindCaptureSpecByDefinition(MaxHealthAttributeCapture, true)) {
		maxHealthSpec->AttemptCalculateAttributeMagnitude(FAggregatorEvaluateParameters(), maxHealth);
	}
	return health / maxHealth;
}

UWitherDamageExecutionCalculation::UWitherDamageExecutionCalculation()
	: HealthAttributeCapture(UHealthAttributeSet::HealthAttribute(), EGameplayEffectAttributeCaptureSource::Target, false)
	, MaxHealthAttributeCapture(UHealthAttributeSet::MaxHealthAttribute(), EGameplayEffectAttributeCaptureSource::Target, false) {
	RelevantAttributesToCapture.Emplace(HealthAttributeCapture);
	RelevantAttributesToCapture.Emplace(MaxHealthAttributeCapture);
}

void UWitherDamageExecutionCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const {
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);
	auto spec = ExecutionParams.GetOwningSpec();
	auto abilitySystem = ExecutionParams.GetTargetAbilitySystemComponent();
	auto effectHandle = wither::getActiveEffectHandle(abilitySystem);
	const auto durationLeft = wither::getDurationLeft(wither::getActiveEffect(abilitySystem, effectHandle), *abilitySystem->GetWorld());
	const auto clampedDurationLeft = wither::clampWitherDuration(GetHealthPercentage(spec), durationLeft);

	if (clampedDurationLeft != durationLeft) {
		wither::setDurationForEffect(abilitySystem, effectHandle, clampedDurationLeft);
	}	
}

float UWitherDamageExecutionCalculation::GetHealthPercentage(const FGameplayEffectSpec& spec) const {
	float health = 1.f;
	if (const FGameplayEffectAttributeCaptureSpec* healthSpec = spec.CapturedRelevantAttributes.FindCaptureSpecByDefinition(HealthAttributeCapture, true)) {
		healthSpec->AttemptCalculateAttributeMagnitude(FAggregatorEvaluateParameters(), health);
	}
	float maxHealth = 1.f;
	if (const FGameplayEffectAttributeCaptureSpec* maxHealthSpec = spec.CapturedRelevantAttributes.FindCaptureSpecByDefinition(MaxHealthAttributeCapture, true)) {
		maxHealthSpec->AttemptCalculateAttributeMagnitude(FAggregatorEvaluateParameters(), maxHealth);
	}
	return health / maxHealth;
}

UWitherApplierGameplayEffect::UWitherApplierGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	DurationMagnitude = durationMagnitude;

	FGameplayEffectExecutionDefinition execution;
	execution.CalculationClass = UWitherApplierExecutionCalculation::StaticClass();
	Executions.Add(std::move(execution));

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Negative")));
}


UWitherGameplayEffect::UWitherGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::HasDuration;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	DurationMagnitude = durationMagnitude;

	bExecutePeriodicEffectOnApplication = true;
	bRequireModifierSuccessToTriggerCues = true;
	bSuppressStackingCues = true;
	Period = wither::PERIOD;

	FAttributeBasedFloat damageMagnitude;
	damageMagnitude.BackingAttribute.AttributeToCapture = UHealthAttributeSet::MaxHealthAttribute();
	damageMagnitude.BackingAttribute.bSnapshot = false;
	damageMagnitude.BackingAttribute.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	damageMagnitude.Coefficient = -wither::DAMAGE_PER_TICK;

	FGameplayModifierInfo info;
	info.Attribute = UHealthAttributeSet::HealthAttribute();
	info.ModifierMagnitude = damageMagnitude;
	info.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(info);

	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UWitherDamageExecutionCalculation::StaticClass();
	damage.PassedInTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Weak")));
	Executions.Add(std::move(damage));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.StatusEffect.Wither")), 0, 1);
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Damage.Weak")), 0, 1);
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Negative")));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Weak")));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Wither")));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Wither")));
}