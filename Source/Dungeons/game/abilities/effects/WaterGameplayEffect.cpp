#include "Dungeons.h"
#include "WaterGameplayEffect.h"
#include "game/abilities/attributes/MovementAttributeSet.h"


UWaterGameplayEffect::UWaterGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bSuppressStackingCues = true;

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Water.Submerged"));
	RemoveGameplayEffectsWithTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Fire"));
	GrantedApplicationImmunityTags.RequireTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Fire"));
}


