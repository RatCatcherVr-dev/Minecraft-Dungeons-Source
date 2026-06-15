#include "Dungeons.h"
#include "BackstabbersBrewInstance.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/player/BasePlayerController.h"
#include <GameplayEffect.h>
#include <AbilitySystemComponent.h>
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/calculations/StatusModCalculations.h"
#include "game/abilities/effects/GameplayEffectUtil.h"

ABackstabbersBrewInstance::ABackstabbersBrewInstance() {
	Effect = UBackstabbersBrewGameplayEffect::StaticClass();
}

int ABackstabbersBrewInstance::GetDisplayCount() const {
	return -1;
}

void ABackstabbersBrewInstance::Activate(const FPredictionKey& predictionKey) {
	const auto characterOwner = Cast<ABaseCharacter>(GetOwner());
	auto abilitySystem = characterOwner->GetAbilitySystemComponent();
	const auto effect = Cast<UBackstabbersBrewGameplayEffect>(Effect->GetDefaultObject());
	
	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
	FGameplayEffectSpec spec(effect, abilitySystem->MakeEffectContext(), ItemPower);
	spec.SetSetByCallerMagnitude(effects::DurationName, game::item::type::BackstabbersBrew.getDurationSeconds());
	spec.SetSetByCallerMagnitude(TEXT("Magnitude"), MeleePowerBoostAmount);
	abilitySystem->ApplyGameplayEffectSpecToSelf(spec, predictionKey);
	Super::Activate(predictionKey);

	characterOwner->OnInvisibilityGained();
}

const FName UBackstabbersBrewGameplayEffect::EffectDurationName = FName("EffectDuration");

UBackstabbersBrewGameplayEffect::UBackstabbersBrewGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::HasDuration;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	DurationMagnitude = durationMagnitude;

	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo dealDamageInfo;
	dealDamageInfo.Attribute = UMeleeAttributeSet::MeleeAttackDamageMultiplierAttribute();

	FSetByCallerFloat dealDamageMagnitude;
	dealDamageMagnitude.DataName = TEXT("Magnitude");
	dealDamageInfo.ModifierMagnitude = dealDamageMagnitude;

	dealDamageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(dealDamageInfo);
	
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Invisibility"), 0, 1);
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.BackstabbersBrew"), 0, 1);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Positive"));

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Melee.Buff.Backstab"));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Invisible"));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("AI.Ignore"));
}
