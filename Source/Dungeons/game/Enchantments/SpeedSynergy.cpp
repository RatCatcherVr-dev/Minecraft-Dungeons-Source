// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "SpeedSynergy.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "../abilities/effects/calculations/StatusModCalculations.h"

namespace {
	FName SpeedSynergyDurationMagnitude(TEXT("SpeedSynergyDurationMagnitude"));
	FName SpeedSynergyMoveSpeedEffectMagnitude(TEXT("SpeedSynergyMoveSpeedEffectMagnitude"));
}

USpeedSynergy::USpeedSynergy() {
	TypeId = EEnchantmentTypeID::SpeedSynergy;
	LevelMultiplier = [](int level) -> float {
		return (float)level;
	};
	MultiplierFormatter = valueformat::asDurationRoundedSecond;
}

FText USpeedSynergy::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asRelativeMultiplierPercentageChange(MovementSpeedMultiplier)));
}

void USpeedSynergy::OnItemSuccess() {
	if (GetOwner()->HasAuthority()) {
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
		BroadcastEnchantmentTriggeredEvent();
		auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
		FGameplayEffectSpec spec(effects::CreateGameplayEffectSpec<USpeedSynergyGameplayEffect>(abilitySystem));
		spec.SetSetByCallerMagnitude(::SpeedSynergyMoveSpeedEffectMagnitude, MovementSpeedMultiplier);
		spec.SetSetByCallerMagnitude(effects::DurationName, LevelMultiplier(Level));
		abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
	}
}

USpeedSynergyGameplayEffect::USpeedSynergyGameplayEffect(const FObjectInitializer& ObjectInitializer)
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

	FGameplayModifierInfo speedInfo;
	speedInfo.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();

	FSetByCallerFloat speedMagnitude;
	speedMagnitude.DataName = ::SpeedSynergyMoveSpeedEffectMagnitude;

	speedInfo.ModifierMagnitude = speedMagnitude;
	speedInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(speedInfo);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Positive"));
	
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.SpeedSynergy"), 0, 1);
}