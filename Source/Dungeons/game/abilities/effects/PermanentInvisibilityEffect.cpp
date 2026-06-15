// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "PermanentInvisibilityEffect.h"

UPermanentInvisibilityEffect::UPermanentInvisibilityEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	bRequireModifierSuccessToTriggerCues = true;

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Invisibility"), 0, 1);
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Invisible"));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("AI.Ignore"));
}
