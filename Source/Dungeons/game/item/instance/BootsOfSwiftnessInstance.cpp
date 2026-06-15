#include "Dungeons.h"
#include "BootsOfSwiftnessInstance.h"
#include "game/component/HealthComponent.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "AbilitySystemComponent.h"
#include "game/item/power/ItemPowerEffectDefs.h"
#include <GameplayEffect.h>
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/calculations/StatusModCalculations.h"
#include "game/abilities/effects/GameplayEffectUtil.h"

namespace {
	FName BootsOfSwiftnessEffectDuration(TEXT("BootsOfSwiftnessDuration"));
	FName BootsOfSwiftnessEffectMagnitude(TEXT("BootsOfSwiftnessMagnitude"));
}

ABootsOfSwiftnessInstance::ABootsOfSwiftnessInstance() {
	Effect = UBootsOfSwiftnessGameplayEffect::StaticClass();
	PowerEffects = { UDurationIncrease::StaticClass() };
	SharedPassiveCooldown = 0.f;
}

int ABootsOfSwiftnessInstance::GetDisplayCount() const {
	// Always return 0 on permanent items, unless it has a special condition like the harvester, makes no number displayed
	return 0;
}

void ABootsOfSwiftnessInstance::Activate(const FPredictionKey& predictionKey) {
	auto owner = GetOwner();
	auto playerOwner = Cast<ABaseCharacter>(owner);
	auto abilitySystem = playerOwner->GetAbilitySystemComponent();
	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
	FGameplayEffectSpec spec(Cast<UBootsOfSwiftnessGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext(), ItemPower);

	const float ItemPowerMultiplier = GetPowerEffect()->GetMultiplier(ItemPower);
	spec.SetSetByCallerMagnitude(effects::DurationName, game::item::type::BootsOfSwiftness.getDurationSeconds()*ItemPowerMultiplier);

	spec.SetSetByCallerMagnitude(::BootsOfSwiftnessEffectMagnitude, SpeedBoostAmount);
	abilitySystem->ApplyGameplayEffectSpecToSelf(spec, predictionKey);
	Super::Activate(predictionKey);
}

UBootsOfSwiftnessGameplayEffect::UBootsOfSwiftnessGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	DurationMagnitude = durationMagnitude;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo info;
	info.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();

	FSetByCallerFloat powerMagnitude;
	powerMagnitude.DataName = ::BootsOfSwiftnessEffectMagnitude;

	info.ModifierMagnitude = powerMagnitude;
	info.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(info);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Positive"));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Swiftness"), 0, 1);
}