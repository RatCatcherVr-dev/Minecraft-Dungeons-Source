#include "Dungeons.h"
#include "DenseBrewPotionInstance.h"

#include "AbilitySystemComponent.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/attributes/ResistanceAttributeSet.h"
#include "game/abilities/effects/requirements/BaseGameplayEffectApplicationRequirement.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/abilities/effects/calculations/StatusModCalculations.h"

UDenseBrewPotionGameplayEffect::UDenseBrewPotionGameplayEffect(const FObjectInitializer& ObjectInitializer) : Super() {
	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	DurationMagnitude = durationMagnitude;

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Positive"));

	effects::AddSetByCallerAttribute(*this, UHealthAttributeSet::TakeDamageMultiplierAttribute(), EGameplayModOp::Division);

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Potion.DenseBrew"), 0, 1);
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.DenseBrew"), 0, 1);
}

// ------------------------------------------------------------------------------------------- //

ADenseBrewInstance::ADenseBrewInstance() {
	Effect = UDenseBrewPotionGameplayEffect::StaticClass();
}

int ADenseBrewInstance::GetDisplayCount() const {
	return -1;
}

void ADenseBrewInstance::Activate(const FPredictionKey& predictionKey) {
	const auto characterOwner = Cast<ABaseCharacter>(GetOwner());
	auto abilitySystem = characterOwner->GetAbilitySystemComponent();
	const auto effect = Cast<UDenseBrewPotionGameplayEffect>(Effect->GetDefaultObject());

	float pushbackResistanceDivider = 1 / Math::max(0.01f, 1 - PushbackResistance);
	float meleeResistanceDivider = 1 / Math::max(0.01f, 1 - MeleeDamageResistance);

	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
	FGameplayEffectSpec spec(effect, abilitySystem->MakeEffectContext(), ItemPower);
	spec.SetSetByCallerMagnitude(effects::DurationName, game::item::type::DenseBrewPotion.getDurationSeconds());
	spec.SetSetByCallerMagnitude(*UResistanceAttributeSet::PushbackResistanceMagnitudeAttribute().GetName(), pushbackResistanceDivider);
	spec.SetSetByCallerMagnitude(*UResistanceAttributeSet::WindResistanceMagnitudeAttribute().GetName(), pushbackResistanceDivider);
	spec.SetSetByCallerMagnitude(*UHealthAttributeSet::TakeDamageMultiplierAttribute().GetName(), meleeResistanceDivider);
	abilitySystem->ApplyGameplayEffectSpecToSelf(spec, predictionKey);

	Super::Activate(predictionKey);
}
