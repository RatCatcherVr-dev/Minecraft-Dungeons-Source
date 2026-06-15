#include "Dungeons.h"
#include "InvulnerableGameplayEffect.h"
#include <GameplayEffect.h>
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/calculations/StatusModCalculations.h"

UInvulnerableNoCueGameplayEffect::UInvulnerableNoCueGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	DurationMagnitude = durationMagnitude;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	DurationMagnitude = durationMagnitude;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo damageInfo;
	damageInfo.Attribute = UHealthAttributeSet::TakeDamageMultiplierAttribute();
	damageInfo.ModifierMagnitude = FScalableFloat(0.f);
	damageInfo.ModifierOp = EGameplayModOp::Type::Override;
	Modifiers.Add(damageInfo);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Positive"));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("Immunity.Damage"));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("Immunity.Damage"));

	GrantedApplicationImmunityTags.RequireTags.AddTag(FGameplayTag::RequestGameplayTag("Damage.Melee"));
	GrantedApplicationImmunityTags.RequireTags.AddTag(FGameplayTag::RequestGameplayTag("Damage.Ranged"));
}

UInvulnerableGameplayEffect::UInvulnerableGameplayEffect() {
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.GhostWalk.Invulnerability"));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Invulnerable"), 0, 1);
}


