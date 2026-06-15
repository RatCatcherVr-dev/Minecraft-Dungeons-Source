#include "Dungeons.h"
#include "StaggerGamePlayEffect.h"
#include <GameplayEffect.h>
#include "requirements/BaseGameplayEffectApplicationRequirement.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "calculations/EnduranceModCalculations.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "calculations/StatusModCalculations.h"



UStaggerGameplayEffect::UStaggerGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	DurationMagnitude = durationMagnitude;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Negative")));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Stunned"));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Stunned"));
}


UEnduranceRecoveryGameplayEffect::UEnduranceRecoveryGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	Period = 0.1f;

	FCustomCalculationBasedFloat endurancePerTick;
	endurancePerTick.CalculationClassMagnitude = URecoverEndurancePerSecondModCalculation::StaticClass();

	FGameplayModifierInfo enduranceModifier;
	enduranceModifier.Attribute = UHealthAttributeSet::EnduranceAttribute();
	enduranceModifier.ModifierOp = EGameplayModOp::Additive;
	enduranceModifier.ModifierMagnitude = endurancePerTick;

	Modifiers.Add(enduranceModifier);
}

UStunImmunityGameplayEffect::UStunImmunityGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	GrantedApplicationImmunityTags.RequireTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Stunned"));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("Immunity.Stun"));
}


UTemporaryStunimmunityGameplayEffect::UTemporaryStunimmunityGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	GrantedApplicationImmunityTags.RequireTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Stunned"));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("Immunity.Stun"));
	
	
	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = FName(TEXT("Duration"));
	DurationMagnitude = durationMagnitude;
}