#include "Dungeons.h"
#include "WaterBreathingPotionInstance.h"

#include "AbilitySystemComponent.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/requirements/BaseGameplayEffectApplicationRequirement.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/GamplayEffects/Drowning.h"
#include <GameplayEffect.h>
#include "game/abilities/effects/calculations/StatusModCalculations.h"

AWaterBreathingPotionInstance::AWaterBreathingPotionInstance()
{
	Effect = UWaterBreathingPotionGameplayEffect::StaticClass();
}

int AWaterBreathingPotionInstance::GetDisplayCount() const
{
	return -1;
}

void AWaterBreathingPotionInstance::Activate(const FPredictionKey& predictionKey)
{
	const auto characterOwner = Cast<ABaseCharacter>(GetOwner());
	auto abilitySystem = characterOwner->GetAbilitySystemComponent();
	const auto effect = Cast<UWaterBreathingPotionGameplayEffect>(Effect->GetDefaultObject());

	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
	FGameplayEffectSpec spec(effect, abilitySystem->MakeEffectContext(), ItemPower);
	spec.SetSetByCallerMagnitude(effects::DurationName, game::item::type::WaterBreathingPotion.getDurationSeconds());
	abilitySystem->ApplyGameplayEffectSpecToSelf(spec, predictionKey);

	Super::Activate(predictionKey);
}

UWaterBreathingPotionGameplayEffect::UWaterBreathingPotionGameplayEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	DurationMagnitude = durationMagnitude;

	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());

	//Replenish oxygen
	FConditionalGameplayEffect oxygenReplenish;
	oxygenReplenish.EffectClass = UOxygenReplenishEffect::StaticClass();
	oxygenReplenish.RequiredSourceTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Underwater"));
	ConditionalGameplayEffects.Add(oxygenReplenish);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Positive"));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Potion.WaterBreathing"), 0, 1);
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.WaterBreathing"), 0, 1);
	const auto waterBreathingEffectTag = FGameplayTag::RequestGameplayTag("StatusEffect.WaterBreathing");
	InheritableOwnedTagsContainer.AddTag(waterBreathingEffectTag);
	InheritableGameplayEffectTags.AddTag(waterBreathingEffectTag);
}