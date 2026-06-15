#include "Dungeons.h"
#include "SwiftnessPotionInstance.h"

#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "AbilitySystemComponent.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include <GameplayEffect.h>
#include "game/abilities/effects/requirements/BaseGameplayEffectApplicationRequirement.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/calculations/StatusModCalculations.h"
#include "game/abilities/effects/GameplayEffectUtil.h"

namespace {
	FName SwiftnessEffectDuration(TEXT("SwiftnessDuration"));
	FName SwiftnessEffectMagnitude(TEXT("SwiftnessMagnitude"));
}

ASwiftnessPotionInstance::ASwiftnessPotionInstance() {
	Effect = USwiftnessPotionGameplayEffect::StaticClass();
}

void ASwiftnessPotionInstance::Activate(const FPredictionKey& predictionKey) {
	auto playerOwner = Cast<APlayerCharacter>(GetOwner());

	auto abilitySystem = playerOwner->GetAbilitySystemComponent();
	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
	FGameplayEffectSpec spec(Cast<USwiftnessPotionGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext(), ItemPower);
	spec.SetSetByCallerMagnitude(effects::DurationName, game::item::type::SwiftnessPotion.getDurationSeconds());
	spec.SetSetByCallerMagnitude(::SwiftnessEffectMagnitude, SpeedBoostAmount);
	abilitySystem->ApplyGameplayEffectSpecToSelf(spec, predictionKey);
	Super::Activate(predictionKey);
}

USwiftnessPotionGameplayEffect::USwiftnessPotionGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	DurationMagnitude = durationMagnitude;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo info;
	info.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();

	FSetByCallerFloat powerMagnitude;
	powerMagnitude.DataName = ::SwiftnessEffectMagnitude;

	info.ModifierMagnitude = powerMagnitude;
	info.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(info);

	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Positive"));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Swiftness"), 0, 1);
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Potion.Swiftness"), 0, 1);
}
