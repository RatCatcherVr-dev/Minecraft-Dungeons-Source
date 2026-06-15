#include "Dungeons.h"
#include "WaterBreathing.h"

#include "AbilitySystemComponent.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/requirements/BaseGameplayEffectApplicationRequirement.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/GamplayEffects/Drowning.h"
#include <GameplayEffect.h>

UWaterBreathingGameplayEffect::UWaterBreathingGameplayEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	StackingType = EGameplayEffectStackingType::AggregateBySource;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;

	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.WaterBreathing"), 0, 1);
	const auto waterBreathingEffectTag = FGameplayTag::RequestGameplayTag("StatusEffect.WaterBreathing");
	InheritableOwnedTagsContainer.AddTag(waterBreathingEffectTag);
	InheritableGameplayEffectTags.AddTag(waterBreathingEffectTag);

	//Replenish oxygen
	FConditionalGameplayEffect oxygenReplenish;
	oxygenReplenish.EffectClass = UOxygenReplenishEffect::StaticClass();
	ConditionalGameplayEffects.Add(oxygenReplenish);
}

UMobWaterBreathingGameplayEffect::UMobWaterBreathingGameplayEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	StackingType = EGameplayEffectStackingType::AggregateBySource;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;

	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());

	OngoingTagRequirements.RequireTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Underwater"));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.MobWaterBreathing"), 0, 1);
	const auto waterBreathingEffectTag = FGameplayTag::RequestGameplayTag("StatusEffect.MobWaterBreathing");
	InheritableOwnedTagsContainer.AddTag(waterBreathingEffectTag);
	InheritableGameplayEffectTags.AddTag(waterBreathingEffectTag);
}