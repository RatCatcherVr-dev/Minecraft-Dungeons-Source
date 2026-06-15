#include "GhostFormGameplayEffect.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/attributes/HealthAttributeSet.h"

const FName UGhostFormBaseGameplayEffect::GhostFormTakeDamageKey(TEXT("GhostFormTakeDamageEffect"));
const float UGhostFormBaseGameplayEffect::DamageTakenMultiplier = 0.5f;

UGhostFormBaseGameplayEffect::UGhostFormBaseGameplayEffect() {
	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo damageInfo;
	damageInfo.Attribute = UHealthAttributeSet::TakeDamageMultiplierAttribute();
	damageInfo.ModifierMagnitude = FScalableFloat(1.0f/DamageTakenMultiplier);
	damageInfo.ModifierOp = EGameplayModOp::Type::Division;

	Modifiers.Add(damageInfo);

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.GhostWalk"));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Ghost"), 0, 1);
}