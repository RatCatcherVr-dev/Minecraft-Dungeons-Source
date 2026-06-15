#include "Dungeons.h"
#include "DefensePotionInstance.h"

#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "AbilitySystemComponent.h"
#include <GameplayEffect.h>
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/requirements/BaseGameplayEffectApplicationRequirement.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/effects/calculations/StatusModCalculations.h"

ADefensePotionInstance::ADefensePotionInstance() {
	Effect = UDefensePotionGameplayEffect::StaticClass();
}

int ADefensePotionInstance::GetDisplayCount() const {
	return -1;
}

void ADefensePotionInstance::Activate(const FPredictionKey& predictionKey) {
	const auto characterOwner = Cast<ABaseCharacter>(GetOwner());
	auto abilitySystem = characterOwner->GetAbilitySystemComponent();
	const auto effect = Cast<UDefensePotionGameplayEffect>(Effect->GetDefaultObject());

	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
	FGameplayEffectSpec spec(effect, abilitySystem->MakeEffectContext(), ItemPower);
	spec.SetSetByCallerMagnitude(effects::DurationName, game::item::type::DefensePotion.getDurationSeconds());
	spec.SetSetByCallerMagnitude(TEXT("Magnitude"), 1.0f/DefenseBoostAmount);
	abilitySystem->ApplyGameplayEffectSpecToSelf(spec, predictionKey);

	Super::Activate(predictionKey);
}

//-------------------------------------------------------------------------------------------

UDefensePotionGameplayEffect::UDefensePotionGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	DurationMagnitude = durationMagnitude;

	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo defenseInfo;
	defenseInfo.Attribute = UHealthAttributeSet::TakeDamageMultiplierAttribute();

	FSetByCallerFloat defenseMagnitude;
	defenseMagnitude.DataName = TEXT("Magnitude");
	defenseInfo.ModifierMagnitude = defenseMagnitude;

	defenseInfo.ModifierOp = EGameplayModOp::Multiplicitive;

	Modifiers.Add( defenseInfo );

	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Positive"));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.DefenceUp"), 0, 1);
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Potion.Defense"), 0, 1);

}


